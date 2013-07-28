#include "LinkedList.h"
#include "MenuMaker.h"

class MenuMakerImpl : public MenuMaker {
  void draw(MenuMakerMenu* menu, int highlight);
  void highlight(int pos);
  void onEntry(int id);
  int selected;
  MenuMakerMenu* m;
public:
  virtual void begin();
};
#include "MenuMaker_menu.h"

void MenuMakerImpl::draw(MenuMakerMenu* menu, int highlight) {
  Serial.print("draw(");
  Serial.print(menu->id);
  Serial.print(",");
  Serial.print(menu->title);
  Serial.println(")");
  
  m = menu;
  selected = highlight;
  
  int n = menu->entries->size();
  for(int i=0;i<n;i++) {
    if (i == selected)
      Serial.print("[");
    else Serial.print(" ");

    Serial.print(menu->entries->get(i)->title);

    if (i == selected)
      Serial.println("]");
    else Serial.println(" ");
  }
}

/**
 * Implement this for your hardware
 **/
void MenuMakerImpl::highlight(int pos) {
  Serial.print("highlight(");
  Serial.print(pos);
  Serial.println(")");
  if (selected == pos)
    return;
  
  selected = pos;
  draw(m, selected);
}

void MenuMakerImpl::onEntry(int id) {
  Serial.print("onEntry(");
  Serial.print(id);
  Serial.println(")");
}

// no instance: 4228


MenuMakerImpl MM; // 4668
void setup() {
  Serial.begin(57600);
  Serial.println("Started");
  MM.begin();
  Serial.print("Menus: ");
  Serial.println(MM.menus->size());
  Serial.println("show...");
  MM.show(MENUMAKER_ID_main);
}

static const char *NAMES[6] = {"up", "down", "left", "right", "select", "back"};
void loop() {
  int c = random(0, 12);
  Serial.println(NAMES[c<6?c:c%2==1?1:0]);
  switch(c) {
    case 6:
    case 8:
    case 10:
      MM.input(MENUMAKER_INPUT_UP);
      break;
    case 7:
    case 9:
    case 11:
      MM.input(MENUMAKER_INPUT_DOWN);
      break;
    default:
      MM.input(c+1);
  }
  delay(500);
}