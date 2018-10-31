#include <enulib/currency.hpp>
#include <enulib/enu.hpp>
#include <vector>

#define ENU_SYMBOL S(4, ENU)  
#define EOS_SYMBOL S(4, EOS)  
#define ENUEOS_SYMBOL S(4, ENUEOS)  


using namespace enumivo;

class ex : public contract {
 public:
  ex(account_name self)
      : contract(self) {}

  void receivedenu(const currency::transfer& transfer);
  void receivedeos(const currency::transfer& transfer);
  void receivedshares(const currency::transfer& transfer);

  void apply(account_name contract, action_name act);

};
