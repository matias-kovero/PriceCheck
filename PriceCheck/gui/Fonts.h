#pragma once

enum class Typografy
{
	Heading = 0,
	HeadingItalic = 1,
	Body = 2,
	BodyBold = 3
};

struct CustomFont
{
	Typografy type;
	string name = "";
	string path = "";
	int size = 14;
	ImFont* font;
};

class Fonts
{
public:
	Fonts();

	std::map<string, ImFont*> loaded;

	void LoadFonts(std::shared_ptr<GameWrapper> gw);
	// Core fonts are named: default and title
	ImFont* GetFont(string name);

private:
	std::vector<CustomFont> supportedFonts;
	int counter = 0;
	std::shared_ptr<GameWrapper> _gw;
};

