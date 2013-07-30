#include "MenuMaker.h"

MenuMakerMenu* MenuMakerMenu::addEntry(int id, const char* title, int type) {
	entries->add(new MenuMakerEntry(id, title, type));
	return this;
}

void MenuMakerMenu::removeEntries() {
	while(entries->size() > 0) {
		MenuMakerEntry* e = entries->pop();
		if (e)
			delete(e);
	}
}

void MenuMakerMenu::removeEntry(int id) {
	int index = indexOfEntryId(id);
	MenuMakerEntry* e = entries->remove(index);
	if (e)
		delete(e);
}

int MenuMakerMenu::indexOfEntryId(int id) {
	int n = entries->size();
	for(int i=0;i<n;i++)
		if (entries->get(i)->id == id)
			return i;
	return -1;
}

bool MenuMakerMenu::isPosSelected(int pos) {
	switch(type) {
		case MENUMAKER_TYPE_SELECT_SINGLE:
			return (entries->get(pos)->id == *((int*)choice));

		case MENUMAKER_TYPE_SELECT_MULTI:
			return ((bool*)choice)[pos];
	}
	
	return false;
}

void MenuMaker::createMenus(int n) {
	// for future use, elemination of LinkedLists in favor of static arrays
}

MenuMakerMenu* MenuMaker::createMenu(int id, const char* title, int type, int numEntries) {
	MenuMakerMenu* m = new MenuMakerMenu(id, title, type, numEntries);
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
	
	switch(menu->type) {
		case MENUMAKER_TYPE_SELECT_SINGLE:
			if( (currentEntry = menu->indexOfEntryId(*(int*)menu->choice)) == -1)
				currentEntry = 0;
			break;
		case MENUMAKER_TYPE_SELECT_MULTI:
		default:
			currentEntry = 0;
			break;
	}
	
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

void MenuMaker::input(int key) {
	if (currentMenu->entries->size()==0 && ! (key == MENUMAKER_INPUT_LEFT || key == MENUMAKER_INPUT_BACK))
		return;
		
	switch(key) {
		case MENUMAKER_INPUT_UP:
				
			if (currentEntry <= 0)
				currentEntry = currentMenu->entries->size()-1;
			// TODO else if (rollover)
			else currentEntry--;
			break;

		case MENUMAKER_INPUT_DOWN:
			if (currentEntry >= currentMenu->entries->size()-1)
				currentEntry = 0;
			else currentEntry++;
			break;
			
		case MENUMAKER_INPUT_LEFT:
			// TODO
			// if in widget:
			// decrease()
			// else if in menu
			input(MENUMAKER_INPUT_BACK);
			return;
			
		case MENUMAKER_INPUT_RIGHT:
			// TODO
			// if in widget:
			// increase()
			// else if in menu
			input(MENUMAKER_INPUT_SELECT);
			return;
			
		case MENUMAKER_INPUT_BACK: {
				if (history->size() <= 0)
					return;
				
				MenuMakerMenu* b = history->pop();
				MenuMakerMenu* oldMenu = currentMenu;
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

				onExit(oldMenu);
				
				return; // return, no break. this is select/back not navigation within the current list;
			}
			
		case MENUMAKER_INPUT_SELECT: 
		{
			MenuMakerEntry* e = currentMenu->entries->get(currentEntry);
			MenuMakerMenu* m;
			switch(e->type) { // Jaguar
				case MENUMAKER_TYPE_SUBMENU:
					m = resolveMenu(e->id);
					onPrepare(m);
					select(m);
					break;
				case MENUMAKER_TYPE_SELECT_SINGLE: {
					int old = *((int*)currentMenu->choice);
					*((int*)currentMenu->choice) = e->id;
					onSelection(currentMenu, &old);
					break; }
				case MENUMAKER_TYPE_SELECT_MULTI: {
					bool old[currentMenu->entries->size()];
					memcpy(old, currentMenu->choice, sizeof(bool) * currentMenu->entries->size());
					((bool*)currentMenu->choice)[currentEntry] = !((bool*)currentMenu->choice)[currentEntry];
					onSelection(currentMenu, old);
					}
					break;
				default:
					onEntry(e->id);
					break;
			}
			return; // return, no break. this is select/back not navigation within the current list
		}
		
		default:
			return;
	}

	highlight(currentEntry);
}

MenuMakerMenu* MenuMaker::resolveMenu(int id) {
	const int n = menus->size();
	for (int i=0; i<n; i++) {
		MenuMakerMenu *m = menus->get(i);
		if (m->id != id)
			continue;
		return m;
	}
	// this is an error!
	while(1);
	return 0;
}

void MenuMakerEntry::setOverlay(const char *text) {
	this->overlay = text;
}
