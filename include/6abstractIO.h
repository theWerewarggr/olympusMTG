#ifndef OLYMPUS_6_ABSTRACT_IO
#define OLYMPUS_6_ABSTRACT_IO

#include <vector>
#include <string>
#include "../Mana/head2_mana.h"

class Abstract_io{
protected:
	int posterY, posterZ;
	int messageY, messageZ;
public:
	static const int BLACK, WHITE, GREY, HIGH1, HIGH2;

	virtual ~Abstract_io(){};
	virtual void getResolution(int& posterYSize, int& posterZSize, bool& hasMouseSupport) const = 0;
	virtual void harmonize(int _posterY, int _posterZ, int _messageY, int _messageZ) {posterY = _posterY; posterZ = _posterZ; messageY = _messageY; messageZ = _messageZ; };

	virtual void fulldisp() const = 0;
	virtual void draw_permanent(int left, int top, int width, int height, char color, bool tapped, bool highlight, bool basicImg) const = 0;
	virtual void print_text(const std::string& text, char color, int y, int z) const = 0;
	virtual void print_text(const char* text, char color, int y, int z) const = 0;
	virtual void disp_mana(Mana mana, int endy, int topz) const = 0;
	virtual void draw_rectangle(char color, int y, int z, int dy, int dz, int linewidth) const = 0;
	virtual bool attack_switch(int leftY, int rightY, int topZ, int arrowlength) const = 0;
	virtual void draw_full_rectangle(char color, int left, int top, int width, int height) const = 0;
	virtual void erase_surface(int left, int top, int width, int height) const = 0;
	virtual void disp_header(int y, int z, int width, int height, const char* name, int life, char phase, bool highlight, Mana pool) const = 0;
	virtual DirectioL get_direction_key() = 0;
	virtual int getInt(int lowerBound, int upperBound) = 0;

	virtual void poster(std::string name, Mana manacost, char color, const char* types,
		std::vector<std::string> lines, int power, int toughness, char frametype, bool watermark) const = 0;
	virtual void disp_cardback(int y, int z) const = 0;
	virtual void message(const char* message) const = 0;
	virtual void message(std::string message) const = 0;
	virtual float gmouseY() = 0;
	virtual float gmouseZ() = 0;
	virtual bool gmouseActive() = 0;
};

class Abstract_ui{
protected:
	Abstract_io* myIO;
	bool mouseSupport;
public:
	static const int ELTYPE_STACK = 0, ELTYPE_LOGBOOK = 1, ELTYPE_OPTIONS = 2, ELTYPE_PERMANENTS = 3;

	Abstract_ui(Abstract_io* IO): myIO(IO){};
	virtual ~Abstract_ui() = default;
	virtual UIElement* declare_element(int typeofelement, char player_id) = 0;

	virtual void get_player_coords(char player_id, int* py, int* pz, int* wide, int* high, int* liby, int* libz, int* gravey, int* gravez, int* exily, int* exilz) = 0;
	//different zones like the stack, the battlefield, the hand a.s.o. will be handled by team members aka _UIElements
	//while displaying the player is the personal responsibility of the UI
	//virtual void resize_zones(); //in later versions, zone might change position and size to occupy the whole space, as in MTGArena.
	virtual bool chooseattackers(PContainer<Creature>& cowards, PContainer<Creature>& warriors, char player_id) = 0;
	virtual void chooseblockers(PContainer<Creature>& defenders, PContainer<Creature>& attackers, UIElement* defenderDisplay, UIElement* attackerDisplay) = 0;
	virtual void clear_opts() = 0;
	virtual UIElement* get_optionzone() = 0;
	virtual Target* iterate(bool needstarget, Player** pl, char returntypeflags) = 0;
	virtual Option* choose_opt(bool sorceryspeed, Option* firstopt, Player* asker, int metapos) = 0;
	virtual void deadzone() = 0;
};

class _UIElement{
private:
	int topZ, leftY;
	int yOffset, zOffset;
	int ySize, zSize;
	int maxItems;
public:
	static const bool horizontal = true;
	static const bool vertical = false;

	_UIElement(int left, int top, int ySize, int zSize, int offset, int maxItems, bool direction);
	void get_coordinates(int itemNb, int* y, int* z) const;
	void get_coordinates(int* y, int* z, int* yOffset, int* zOffset) const;
	void get_coordinates(int* y, int* z, int* yOff, int* zOff, int* yWidth, int* zHeight) const;
	void erase_background(Abstract_io* io) const;
	template <class T>
	void disp_all(const PContainer<T>* const objects) const{
		erase_background(god.myIO);
		int y = leftY;
		int z = topZ;
		for(auto iter = objects->cbegin(); iter != objects->cend(); iter++){
			iter->disp(y, z, ySize, zSize, false);
			y += yOffset;
			z += zOffset;
		}
	}
};

typedef _UIElement UIElement;

Abstract_io* new_IOLib();
Abstract_ui* new_UILib(Abstract_io* IOLib);

inline char main_color(char color){
    switch(color){
        case 0: return 0;
        case 1: return 1;
        case 2: return 2;
        case 4: return 3;
        case 8: return 4;
        case 16: return 5;
        default: return 6;
    }
}

#endif //OLYMPUS_6_ABSTRACT_IO