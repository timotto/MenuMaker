#include "MenuMaker.h"

MenuMakerMenu* MenuMakerMenu::addEntry(int id, const char* title, bool submenu) {
	entries->add(new MenuMakerEntry(id, title, submenu));
	return this;
}

MenuMakerMenu* MenuMaker::createMenu(int id, const char* title) {
	MenuMakerMenu* m = new MenuMakerMenu(id, title);
	menus->add(m);
	return m;
}

/**
 * this comes from user space
 **/
void MenuMaker::show(int id) {
	select(resolveMenu(id));
}

void MenuMaker::redraw() {
	draw(currentMenu, currentEntry);
}

void MenuMaker::select(MenuMakerMenu *menu) {
	if (currentMenu != 0) {
		history->add(currentMenu);
	}
	
	currentEntry = 0;
	currentMenu = menu;
	
	redraw();
}

void MenuMaker::onHighlight(int index) {
	if (index < 0)
		index=0;
	
	if (index >= currentMenu->entries->size())
		index = currentMenu->entries->size()-1;
	
	if (currentEntry != index) {
		currentEntry = index;
		highlight(currentEntry);
	}
}

void MenuMaker::onUp(){
	if (currentEntry <= 0)
		currentEntry = currentMenu->entries->size()-1;
	else currentEntry--;
	
	highlight(currentEntry);
}

void MenuMaker::onDown(){
	if (currentEntry >= currentMenu->entries->size()-1)
		currentEntry = 0;
	else currentEntry++;
	
	highlight(currentEntry);
}

void MenuMaker::onLeft(){
	// TODO
	// if in widget:
	// decrease()
	// if in menu
	onBack();
}

void MenuMaker::onRight(){
	// TODO
	// if in widget:
	// increase()
	// if in menu
	onSelect();
}

void MenuMaker::onSelect(){
	MenuMakerEntry* e = currentMenu->entries->get(currentEntry);
	if (e->submenu) {
		select(resolveMenu(e->id));
		return;
	}
	onEntry(e->id);
}

void MenuMaker::onBack() {
	if (history->size() <= 0)
		return;
	
	MenuMakerMenu* b = history->pop();
	const int oid = currentMenu->id;
	int from = 0;
	const int n = b->entries->size();
	for (int i=0;i<n;i++) {
		if (b->entries->get(i)->id == oid) {
			from = i;
			break;
		}
	}
	
	currentEntry = from;
	currentMenu = b;
	redraw();
}

MenuMakerMenu* MenuMaker::resolveMenu(int id) {
	const int n = menus->size();
	for (int i=0; i<n; i++) {
		MenuMakerMenu *m = menus->get(i);
		if (m->id != id)
			continue;
		return m;
	}
	Serial.print("*ERROR* Failed to resolve menu id ");
	Serial.println(id);
	return 0;
}

MenuMaker::MenuMaker() : currentMenu(0) {
	menus = new LinkedList<MenuMakerMenu*>();
	history = new LinkedList<MenuMakerMenu*>();
	onCreate();
};

MenuMakerMenu::MenuMakerMenu(int id, const char* title) : id(id) {
	entries = new LinkedList<MenuMakerEntry*>();
	this->title = title;
//	this->title = (char*)malloc(strlen(title)+1);
//	strcpy(this->title, title);
}

MenuMakerEntry::MenuMakerEntry(int id, const char *title, bool submenu) : id(id), submenu(submenu), overlay(0) {
	this->title = title;
//	this->title = (char*)malloc(strlen(title)+1);
//	strcpy(this->title, title);
};

void MenuMakerEntry::setOverlay(const char *text) {
	this->overlay = text;
}
