////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//                              Title of the Game                                     //
//                                  Menu.hpp                                          //
//                                Ross Maspero                                        //
//                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////


#ifndef MENU_HPP
#define MENU_HPP


#include <entityx/entityx.h>
namespace ex = entityx;


class MenuSystem : public ex::System<MenuSystem> {
   public:
      explicit MenuSystem(ex::EntityManager& entM);
      void update(ex::EntityManager&, ex::EventManager&, ex::TimeDelta) override;
};




#endif // MENU_HPP
