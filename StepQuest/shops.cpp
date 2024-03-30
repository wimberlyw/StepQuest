#include "shops.h"
#include "popup.h"

Location item_1 = {.x1=50,.x2=180,.y1=70,.y2=100};
Location item_2 = {.x1=50,.x2=180,.y1=120,.y2=140};
Location item_3 = {.x1=50,.x2=180,.y1=160,.y2=190};
Location exit_but_shop = {.x1=30,.x2=180,.y1=200,.y2=240};
Location shopTrash1 = {.x1=15,.x2=40,.y1=70,.y2=100};
Location shopTrash2 = {.x1=15,.x2=40,.y1=110,.y2=140};
Location shopTrash3 = {.x1=15,.x2=40,.y1=150,.y2=190};
Location shopLocations[7] = {exit_but_shop,shopTrash1,shopTrash2,shopTrash3,item_1,item_2,item_3};

// Invalid item
Item invalid_item = {.itemName="Empty.",.type=-1,.level=0,.price=0,.obtainable=true,.place=-1};

// Items for town 1
Item s1_1 = {.itemName="Wooden Sword",.type=0,.level=2,.price=25,.obtainable=true,.place=0};
Item s2_1 = {.itemName="Dull Stone Sword",.type=0,.level=4,.price=40,.obtainable=true,.place=1};
Item s3_1 = {.itemName="Stone Sword",.type=0,.level=7,.price=75,.obtainable=true,.place=2};
Item s4_1 = {.itemName="Rusty Iron Sword",.type=0,.level=10,.price=125,.obtainable=true,.place=3};
Item a2_1 = {.itemName="Leather Helmet",.type=1,.level=2,.price=25,.obtainable=true,.place=4};
Item a3_1 = {.itemName="Leather Chestpiece",.type=2,.level=3,.price=45,.obtainable=true,.place=5};
Item a4_1 = {.itemName="Studded Leather Helmet",.type=1,.level=4,.price=60,.obtainable=true,.place=6};
Item a5_1 = {.itemName="Studded Leather Chestpiece",.type=2,.level=6,.price=100,.obtainable=true,.place=7};
Item s1[8] = {s1_1,s2_1,s3_1,s4_1,a2_1,a3_1,a4_1,a5_1}; // shop items for town 1

// items for town 2
Item s0_2 = {.itemName="Sharpened Stone Sword",.type=0,.level=9,.price=100,.obtainable=true,.place=0};
Item s1_2 = {.itemName="Iron Sword",.type=0,.level=12,.price=160,.obtainable=true,.place=1};
Item s2_2 = {.itemName="Dull Steel Sword",.type=0,.level=13,.price=180,.obtainable=true,.place=2};
Item s3_2 = {.itemName="Blunt Iron Sword",.type=0,.level=11,.price=145,.obtainable=true,.place=3};
Item s4_2 = {.itemName="Old Bronze Sword",.type=0,.level=10,.price=135,.obtainable=true,.place=4};
Item s5_2 = {.itemName="Bronze Sword",.type=0,.level=12,.price=160,.obtainable=true,.place=5};
Item s6_2 = {.itemName="Steel Sword",.type=0,.level=15,.price=230,.obtainable=true,.place=6};
Item s7_2 = {.itemName="Honed Iron Sword",.type=0,.level=14,.price=200,.obtainable=true,.place=7};
Item s8_2 = {.itemName="Enchanted Iron Sword",.type=0,.level=18,.price=450,.obtainable=true,.place=8};
Item s9_2 = {.itemName="Enchanted Bronze Sword",.type=0,.level=16,.price=300,.obtainable=true,.place=9};
Item a0_2 = {.itemName="Hide Helmet",.type=1,.level=5,.price=75,.obtainable=true,.place=10};
Item a1_2 = {.itemName="Hide Chestpiece",.type=2,.level=7,.price=125,.obtainable=true,.place=11};
Item a2_2 = {.itemName="Chain Helmet",.type=1,.level=7,.price=130,.obtainable=true,.place=12};
Item a3_2 = {.itemName="Chain Chestpiece",.type=2,.level=9,.price=160,.obtainable=true,.place=13};
Item a4_2 = {.itemName="Scale Helmet",.type=1,.level=9,.price=175,.obtainable=true,.place=14};
Item a5_2 = {.itemName="Scale Breastplate",.type=2,.level=11,.price=220,.obtainable=true,.place=15};
Item a6_2 = {.itemName="Enchanted Scale Breastplate",.type=2,.level=15,.price=400,.obtainable=true,.place=16};
Item s2[17] = {s0_2,s1_2,s2_2,s3_2,s4_2,s5_2,s6_2,s7_2,s8_2,s9_2,a0_2,a1_2,a2_2,a3_2,a4_2,a5_2,a6_2};

// Items for town 3
Item s0_3 = {.itemName="Sharpened Steel Blade",.type=0,.level=18,.price=300,.obtainable=true,.place=0};
Item s1_3 = {.itemName="Enchanted Steel Blade",.type=0,.level=25,.price=600,.obtainable=true,.place=1};
Item s2_3 = {.itemName="Titanium Blade",.type=0,.level=20,.price=360,.obtainable=true,.place=2};
Item s3_3 = {.itemName="Honed Titanium Blade",.type=0,.level=24,.price=450,.obtainable=true,.place=3};
Item s4_3 = {.itemName="Enchanted Titanium Blade",.type=0,.level=30,.price=750,.obtainable=true,.place=4};
Item s5_3 = {.itemName="Razor-Edged Obsidian Blade",.type=0,.level=28,.price=625,.obtainable=true,.place=5};
Item s6_3 = {.itemName="Enchanted Obsidian Blade",.type=0,.level=32,.price=850,.obtainable=true,.place=6};
Item s7_3 = {.itemName="Gleaming Ruby Blade",.type=0,.level=30,.price=675,.obtainable=true,.place=7};
Item s8_3 = {.itemName="Enchanted Ruby Blade",.type=0,.level=35,.price=900,.obtainable=true,.place=8};
Item s9_3 = {.itemName="Sparkling Emerald Blade",.type=0,.level=34,.price=900,.obtainable=true,.place=9};
Item s10_3 = {.itemName="Enchanted Emerald Blade",.type=0,.level=38,.price=1100,.obtainable=true,.place=10};
Item s11_3 = {.itemName="Blinding Diamond Blade",.type=0,.level=40,.price=1200,.obtainable=true,.place=11};
Item s12_3 = {.itemName="Enchanted Diamond Blade",.type=0,.level=45,.price=1500,.obtainable=true,.place=12};
Item s13_3 = {.itemName="The All-Powerful Blade",.type=0,.level=50,.price=2000,.obtainable=true,.place=13};
Item a0_3 = {.itemName="Bronze Helmet",.type=1,.level=12,.price=220,.obtainable=true,.place=14};
Item a1_3 = {.itemName="Bronze Breastplate",.type=2,.level=14,.price=280,.obtainable=true,.place=15};
Item a2_3 = {.itemName="Iron Helmet",.type=1,.level=14,.price=260,.obtainable=true,.place=16};
Item a3_3 = {.itemName="Iron Breastplate",.type=2,.level=16,.price=320,.obtainable=true,.place=17};
Item a4_3 = {.itemName="Steel Helmet",.type=1,.level=16,.price=320,.obtainable=true,.place=18};
Item a5_3 = {.itemName="Steel Breastplate",.type=2,.level=18,.price=380,.obtainable=true,.place=19};
Item a6_3 = {.itemName="Enchanted Iron Breastplate",.type=2,.level=19,.price=420,.obtainable=true,.place=20};
Item a7_3 = {.itemName="Enchanted Steel Helmet",.type=1,.level=19,.price=410,.obtainable=true,.place=21};
Item a8_3 = {.itemName="Ruby Encrusted Helmet",.type=1,.level=20,.price=450,.obtainable=true,.place=22};
Item a9_3 = {.itemName="Ruby Encrusted Breastplate",.type=2,.level=24,.price=600,.obtainable=true,.place=23};
Item a10_3 = {.itemName="Diamond Encrusted Helmet",.type=1,.level=24,.price=700,.obtainable=true,.place=24};
Item a11_3 = {.itemName="Diamond Encrusted Breastplate",.type=2,.level=30,.price=900,.obtainable=true,.place=25};
Item a12_3 = {.itemName="The All-Protecting Breastplate",.type=2,.level=50,.price=2500,.obtainable=true,.place=26};
Item a13_3 = {.itemName="The All-Protecting Helmet",.type=1,.level=48,.price=2000,.obtainable=true,.place=27};
Item a14_3 = {.itemName="Enchanted Diamond Helmet",.type=1,.level=35,.price=1400,.obtainable=true,.place=28};
Item a15_3 = {.itemName="Enchanted Diamond Breastplate",.type=2,.level=40,.price=1700,.obtainable=true,.place=29};
Item s3[30] = {s0_3,s1_3,s2_3,s3_3,s4_3,s5_3,s6_3,s7_3,s8_3,s9_3,s10_3,s11_3,s12_3,s13_3,a0_3,a1_3,a2_3,a3_3,a4_3,a5_3,a6_3,a7_3,a8_3,a9_3,a10_3,a11_3,a12_3,a13_3,a14_3,a15_3};

Item* shop[] = {s1,s2,s3};
int shop_size[] = {8,17,30};

extern TFT_eSprite background;
extern boolean shopDisplayed;
extern Town t;
extern Player p;

Item selectItem()
{
  // need to know which location to pull from
  int store = t.location;

  // pick a random item
  int pick = random(shop_size[store]);
  // is this item still available?
  if (shop[store][pick].obtainable)
  {
    shop[store][pick].obtainable = false;
    return shop[store][pick];
  }

  // item wasn't available! Loop through all items and return first available item
  for (int i = 0; i < shop_size[store]; i++)
  {
    if(shop[store][i].obtainable)
    {
      shop[store][i].obtainable = false;
      return shop[store][i];
    }
  }

  // There are no more items in this shop
  return invalid_item;
}

void returnShopItem(Item item)
{
  int store = t.location;

  shop[store][item.place].obtainable = true;
}

boolean buyItem(Item item)
{
  // invalid placeholder item
  if (item.type == -1) return false;

  int prev_level;
  String s = "Do you want to purchase " + item.itemName + " for " + item.price + " gold?";
  boolean ans = createYesNoPopup(s);
  s = "";

  if (ans)
  {
    // can we afford it?
    if (p.gold >= item.price)
    {
      p.gold -= item.price;

      prev_level = p.itemLevels[item.type];
      if (p.itemLevels[item.type] <= item.level)
      {
        s = "You have replaced " + p.items[item.type].itemName + " with " + item.itemName + ". ";
        p.itemLevels[item.type] = item.level;
        p.items[item.type] = item;
      }

      s = s + "You have gained " + (p.itemLevels[item.type]-prev_level) + (item.type==0? " power." : " armor.");
      createPopup(s);
      return true;
    }
    else
    {
      if (p.gold == 0) // doesn't print when 0 for some reason
      {
        s = "You cannot afford this item! You have 0";
      }
      else
      {
        s = "You cannot afford this item! You have " + p.gold; 
      }
      s = s + " gold but you need ";
      s = s + item.price;
      s = s + " gold!";
      createPopup(s);
      return false;
    }
  }
}

void checkShopLocation(int x, int y)
{
  for (int i = 0; i < 7; i++)
  {
    if (shopLocations[i].x1 <= x && shopLocations[i].x2 >= x)
    {
      if (shopLocations[i].y1 <= y && shopLocations[i].y2 >= y)
      {
        switch(i)
        {
          case(0):
          {
            shopDisplayed = false;
            break;
          } // end case 0
          case(1): // trash item 1
          {
            String s = "";
            if (p.itemRerolls[t.location] > 0)
            {
              s = "Are you sure you want to reroll this shop item? You will have ";
              s = s + (p.itemRerolls[t.location]-1);
              s = s + " shop items left. (Items reset at 12:00)";
            }
            else return;

            boolean ans = createYesNoPopup(s); 

            if (ans)
            {
              returnShopItem(t.shopItems[0]);
              if (p.itemRerolls[t.location] > 0)
              {
                t.shopItems[0] = selectItem();
                p.itemRerolls[t.location]--;
              }
              else
              {
                t.shopItems[0] = invalid_item;
              }
            }
            break;
          } // end case 1
          case(2): // trash item 2
          {
            String s = "";
            if (p.itemRerolls[t.location] > 0)
            {
              s = "Are you sure you want to reroll this shop item? You will have ";
              s = s + (p.itemRerolls[t.location]-1);
              s = s + " shop items left. (Items reset at 12:00)";
            }
            else return;

            boolean ans = createYesNoPopup(s); 

            if (ans)
            {
              returnShopItem(t.shopItems[1]);
              if (p.itemRerolls[t.location] > 0)
              {
                t.shopItems[1] = selectItem();
                p.itemRerolls[t.location]--;
              }
              else
              {
                t.shopItems[1] = invalid_item;
              }
            }
            break;
          } // end case 2
          case(3): // trash item 3
          {
            String s = "";
            if (p.itemRerolls[t.location] > 0)
            {
              s = "Are you sure you want to reroll this shop item? You will have ";
              s = s + (p.itemRerolls[t.location]-1);
              s = s + " shop items left. (Items reset at 12:00)";
            }
            else return;

            boolean ans = createYesNoPopup(s); 
            
            if (ans)
            {
              returnShopItem(t.shopItems[2]);
              if (p.itemRerolls[t.location] > 0)
              {
                t.shopItems[2] = selectItem();
                p.itemRerolls[t.location]--;
              }
              else
              {
                t.shopItems[2] = invalid_item;
              }
            }
            break;
          } // end case 3
          case(4): // buy item 1
          {
            if (!buyItem(t.shopItems[0])) return;
            if (p.itemRerolls[t.location] > 0)
            {
              t.shopItems[0] = selectItem();
              p.itemRerolls[t.location]--;
            }
            else
            {
              t.shopItems[0] = invalid_item;
            }
            
            break;
          } // end case 4
          case(5): // buy item 2
          {
            if (!buyItem(t.shopItems[1])) return;
            if (p.itemRerolls[t.location] > 0)
            {
              t.shopItems[1] = selectItem();
              p.itemRerolls[t.location]--;
            }
            else
            {
              t.shopItems[1] = invalid_item;
            }
            break;
          } // end case 5
          case(6): // buy item 3
          {
            if (!buyItem(t.shopItems[2])) return;
            if (p.itemRerolls[t.location] > 0)
            {
              t.shopItems[2] = selectItem();
              p.itemRerolls[t.location]--;
            }
            else
            {
              t.shopItems[2] = invalid_item;
            }
            break;
          } // end case 6
        }
      }
    }
  }
}

void displayItems()
{
  background.setTextSize(1);
  background.setTextColor(TFT_BLACK);
  for (int i = 0; i < 3; i++)
  {
    switch(i)
    {
      case(0):
      {
        background.setCursor(65,55);
        background.print(t.shopItems[i].itemName);
        background.setCursor(65,75);
        background.print("Gold: ");
        background.print(t.shopItems[i].price);
        if (t.shopItems[i].type == 0)
        {
          background.print(" Power: ");
        }
        else
        {
          background.print(" Armor: ");
        }
        background.print(t.shopItems[i].level);
        break;
      }
      case(1):
      {
        background.setCursor(65,105);
        background.print(t.shopItems[i].itemName);
        background.setCursor(65,125);
        background.print("Gold: ");
        background.print(t.shopItems[i].price);
        if (t.shopItems[i].type == 0)
        {
          background.print(" Power: ");
        }
        else
        {
          background.print(" Armor: ");
        }
        background.print(t.shopItems[i].level);
        break;
      }
      case(2):
      {
        background.setCursor(65,155);
        background.print(t.shopItems[i].itemName);
        background.setCursor(65,175);
        background.print("Gold: ");
        background.print(t.shopItems[i].price);
        if (t.shopItems[i].type == 0)
        {
          background.print(" Power: ");
        }
        else
        {
          background.print(" Armor: ");
        }
        background.print(t.shopItems[i].level);
        break;
      }
    }
  }
}


Item selectDungeonItem(int level)
{
  // pick a random store
  int sto = random(0, level); // random from the shop array  
  
  // pick a random item
  int pick = random(shop_size[sto]);
    
  return shop[sto][pick];
}


void recallShop()
{
  // no shop to recall
  if (p.path != -1 || p.location == 2 || p.location == 4) return;
  
  for (int i = 0; i < 3; i++)
  {
    p.shopItems[t.location][i] = t.shopItems[i];
  }
  p.shopItemsRem[t.location] = true;
}
