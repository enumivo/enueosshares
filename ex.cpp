#include "ex.hpp"

#include <cmath>
#include <enulib/action.hpp>
#include <enulib/asset.hpp>
#include "enu.token.hpp"

using namespace enumivo;
using namespace std;

void ex::receivedenu(const currency::transfer &transfer) {
  if (transfer.to != _self) {
    return;
  }

  // get ENU balance
  double enu_balance = enumivo::token(N(enu.token)).
	   get_balance(N(enu.eos.mm), enumivo::symbol_type(ENU_SYMBOL).name()).amount;
  enu_balance = enu_balance/10000;

  // get EOS balance
  double eos_balance = enumivo::token(N(stable.coin)).
	   get_balance(N(enu.eos.mm), enumivo::symbol_type(EOS_SYMBOL).name()).amount;
  eos_balance = eos_balance/10000;

  // get total shares
  double shares = enumivo::token(N(shares.coin)).
	   get_supply(enumivo::symbol_type(ENUEOS_SYMBOL).name()).amount;
  shares = shares/10000;

  double received = transfer.quantity.amount;
  received = received/10000;

  double a = eos_balance;
  double b = 2 * eos_balance * enu_balance;
  double c = - eos_balance * eos_balance * received;

  double eu = (sqrt((b*b)-(4*a*c)) - b)/(2*a);
  double ee = received - eu;

  double new_shares = shares * (ee/(eu+enu_balance));

  auto quantity = asset(10000*new_shares, ENUEOS_SYMBOL);

  action(permission_level{_self, N(active)}, N(shares.coin), N(issue),
         std::make_tuple(transfer.from, quantity,
                         std::string("Issue new ENUEOS shares")))
      .send();

  action(permission_level{_self, N(active)}, N(enu.token), N(transfer),
         std::make_tuple(_self, N(enu.eos.mm), transfer.quantity,
                         std::string("Invest ENUEOS shares with ENU")))
      .send();
}

void ex::receivedeos(const currency::transfer &transfer) {
  if (transfer.to != _self) {
    return;
  }

  // get ENU balance
  double enu_balance = enumivo::token(N(enu.token)).
	   get_balance(N(enu.eos.mm), enumivo::symbol_type(ENU_SYMBOL).name()).amount;
  enu_balance = enu_balance/10000;

  // get EOS balance
  double eos_balance = enumivo::token(N(stable.coin)).
	   get_balance(N(enu.eos.mm), enumivo::symbol_type(EOS_SYMBOL).name()).amount;
  eos_balance = eos_balance/10000;

  // get total shares
  double shares = enumivo::token(N(shares.coin)).
	   get_supply(enumivo::symbol_type(ENUEOS_SYMBOL).name()).amount;
  shares = shares/10000;

  double received = transfer.quantity.amount;
  received = received/10000;

  double a = enu_balance;
  double b = 2 * enu_balance * eos_balance;
  double c = - enu_balance * enu_balance * received;

  double ue = (sqrt((b*b)-(4*a*c)) - b)/(2*a);
  double uu = received - ue;

  double new_shares = shares * (uu/(ue+eos_balance));

  auto quantity = asset(10000*new_shares, ENUEOS_SYMBOL);

  action(permission_level{_self, N(active)}, N(shares.coin), N(issue),
         std::make_tuple(transfer.from, quantity,
                         std::string("Issue new ENUEOS shares")))
      .send();

  action(permission_level{_self, N(active)}, N(stable.coin), N(transfer),
         std::make_tuple(_self, N(enu.eos.mm), transfer.quantity,
                         std::string("Invest ENUEOS shares with ENU")))
      .send();
}

void ex::receivedshares(const currency::transfer &transfer) {
  if (transfer.to != _self) {
    return;
  }

  // get ENU balance
  double enu_balance = enumivo::token(N(enu.token)).
	   get_balance(N(enu.eos.mm), enumivo::symbol_type(ENU_SYMBOL).name()).amount;
  enu_balance = enu_balance/10000;

  // get EOS balance
  double eos_balance = enumivo::token(N(stable.coin)).
	   get_balance(N(enu.eos.mm), enumivo::symbol_type(EOS_SYMBOL).name()).amount;
  eos_balance = eos_balance/10000;

  // get total shares
  double shares = enumivo::token(N(shares.coin)).
	   get_supply(enumivo::symbol_type(ENUEOS_SYMBOL).name()).amount;
  shares = shares/10000;

  double received = transfer.quantity.amount;
  received = received/10000;

  double eosportion = eos_balance*(received/shares);
  auto eos = asset(10000*eosportion, EOS_SYMBOL);

  action(permission_level{N(enu.eos.mm), N(active)}, N(stable.coin), N(transfer),
         std::make_tuple(N(enu.eos.mm), transfer.from, eos,
                         std::string("Divest ENUEOS shares for EOS")))
      .send();

  double enuportion = enu_balance*(received/shares);
  auto enu = asset(10000*enuportion, ENU_SYMBOL);

  action(permission_level{N(enu.eos.mm), N(active)}, N(enu.token), N(transfer),
         std::make_tuple(N(enu.eos.mm), transfer.from, enu,
                         std::string("Divest ENUEOS shares for ENU")))
      .send();

  action(permission_level{_self, N(active)}, N(shares.coin), N(retire),
         std::make_tuple(transfer.quantity, std::string("Retire ENUEOS shares")))
      .send();
}

void ex::apply(account_name contract, action_name act) {

  if (contract == N(enu.token) && act == N(transfer)) {
    auto transfer = unpack_action_data<currency::transfer>();

    enumivo_assert(transfer.quantity.symbol == ENU_SYMBOL,
                 "Must send ENU");
    receivedenu(transfer);
    return;
  }

  if (contract == N(stable.coin) && act == N(transfer)) {
    auto transfer = unpack_action_data<currency::transfer>();

    enumivo_assert(transfer.quantity.symbol == EOS_SYMBOL,
                 "Must send EOS");
    receivedeos(transfer);
    return;
  }

  if (contract == N(shares.coin) && act == N(transfer)) {
    auto transfer = unpack_action_data<currency::transfer>();

    enumivo_assert(transfer.quantity.symbol == ENUEOS_SYMBOL,
                 "Must send ENUEOS");
    receivedshares(transfer);
    return;
  }

  if (act == N(transfer)) {
    auto transfer = unpack_action_data<currency::transfer>();
    enumivo_assert(false, "Must send EOS or ENU or ENUEOS");
    return;
  }

  if (contract != _self) return;

}

extern "C" {
[[noreturn]] void apply(uint64_t receiver, uint64_t code, uint64_t action) {
  ex enueosshares(receiver);
  enueosshares.apply(code, action);
  enumivo_exit(0);
}
}
