#include ".header_link.h"
#include "../HFiles/headR_readfiles.h"
#include "headU_utilities.h"
#include "../HFiles/9modifs.h" //there seems to be an implicit deletion of Card which implies deleting a Targeter or something?

#define OLYMPUS_BINARYCOMPAT_VERSION 1

void check_safepoint(std::ifstream& myfile, char c, const char* message){
	char x = myfile.get();
	if(x != c){
		raise_error(std::string(message) + ": expected '" + c + "', got '" + x + "' instead");
	}
}

Player::Player(const char* deck_name, char id): Target(&name), Damageable(20), name{0}, state(0xe0), player_id(id), permUI{0}, optZone(0){
	target_flags = 0x80;

	std::string cacheName = std::string("Materials/decks/.binary/deck") + (char)(id + '0');
	std::ifstream inputCache(cacheName, std::ios::binary | std::ios::in);
    int version = 0;
	if(inputCache.is_open()) inputCache.read((char*) &version, sizeof(int));
	if(version == OLYMPUS_BINARYCOMPAT_VERSION){
	    std::cout << "Binary preformatted file found!" << std::endl;
		char x;
		check_canary('<', inputCache);
		inputCache.read(&x, sizeof(char));
		char name_tmp[(int) x];
		inputCache.read(name_tmp, x * sizeof(char));
		name = std::string(name_tmp);
		check_canary('-', inputCache);

		myzones[0].init(inputCache);
		inputCache.close();
	}
	else{
		std::cout << "Binary file not found.";
		std::ifstream myfile(std::string("Materials/decks/") + deck_name + ".mtgoly");
		if(!myfile.is_open()) raise_error("Deck name not found");

		std::ofstream outputCache(".tmp", std::ios::binary | std::ios::out);
		int i = OLYMPUS_BINARYCOMPAT_VERSION;
		outputCache.write((char*) &i, sizeof(int));
		set_canary('<', outputCache);

		std::cout << "Interpreting plain text deck..." << std::endl;
		myfile.get(); //<
		char tmp[11];
		myfile.get(tmp, 10, '>'); //get max. 10 characters, without ending '>'
		name = std::string(tmp);
		char x; for(x = 0; tmp[(int) x] != '\0'; x++);
		outputCache.write(&x, sizeof(char));
		outputCache.write(tmp, x * sizeof(char));
		set_canary('-', outputCache);

		myfile.get(); // newline
		myzones[0].init(myfile, outputCache);
		outputCache.close();
		rename(".tmp", cacheName.c_str());
		myfile.close();
	}

	myzones[0].init_name("Library");
	myzones[1].init_name("Graveyard");
	myzones[2].init_name("Exile");

	myzones[0].shuffle();
	possiblepool = 0;
	manapool = 0;
	draw(7);

	optZone = god.myUI->get_optionzone();
	for(int i=0; i<5; i++){
		permUI[i] = god.myUI->declare_element(i + Abstract_ui::ELTYPE_PERMANENTS, player_id);
	}
}

void CardZone::init(std::ifstream& myfile, std::ofstream& binaryLog){
	size = 0;
	set_canary('o', binaryLog);
	while(myfile.peek() != EOF){ //EOF char not encountered
		char nb = 0;
		char cardname[30];
		int nb_i;
		myfile >> std::skipws >> nb_i; nb = (char) nb_i; //getting number of cards
		check_safepoint(myfile, ' ', "before card name");
		check_safepoint(myfile, '<', "before card name"); //<
		myfile.get(cardname, 30, '>'); //get max. 30 characters, ending with '>'
		check_safepoint(myfile, '>', "after card name"); //space

		char tmp[20];
		myfile >> tmp; //should end with ' '
		Mana cost(tmp);
		char typeOfCard;
		myfile >> typeOfCard;

		char type;
		switch(typeOfCard){
			case 'I': type=0; break;
			case 'L': type=1; break;
			case 'A': type=2; break;
			case 'P': type=3; break;
			case 'C': type=4; break;
			case 'S': type=5; break;
			default: raise_error(std::string("while reading decks: ") + typeOfCard + " is not a card type");
		}
		binaryLog.write(&nb, sizeof(char));
		std::shared_ptr<CardOracle> cardrules = std::make_unique<CardOracle>(myfile, cardname, cost, type); //stops after newline
		cardrules->write_binary(binaryLog);
		for(char i=0; i<nb; i++){
			Card* first = new Card(cardrules);
			cards.push_front(first);
			size++;
		}
		god.gdebug(DBG_X_READFILE) << (int) nb << "cards of type " << typeOfCard << " named " << cardname << " created" << std::endl;
	}
	char x = 0; //! zero cards means 'end of file'
	binaryLog.write(&x, sizeof(char));
	set_canary('m', binaryLog);
}