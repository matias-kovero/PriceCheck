#include "pch.h"
#include "Fonts.h"
#include "bakkesmod/wrappers/GuiManagerWrapper.h"

Fonts::Fonts()
{
  // Path is relative to "bakkesmod/data/fonts/"
  supportedFonts = {
    { Typografy::HeadingItalic, "RLHeadI", "Arvin Bold Italic.ttf", 20, nullptr },
  };
}

void Fonts::LoadFonts(std::shared_ptr<GameWrapper> gw)
{
  auto gui = gw->GetGUIManager();

  for (const auto& f : supportedFonts)
  {
    auto [res, font] = gui.LoadFont(f.name, f.path, f.size);

		if (res == 1) LOG("Font {} will be loaded", f.name);
		else if (res == 0) LOG("Failed to load font: {}", f.name);
		else if (res == 2 && font)
		{
      counter++;
      loaded.insert(std::pair<string, ImFont*>(f.name, font));
		}
  }
}

ImFont* Fonts::GetFont(string name)
{
  if (const auto it = loaded.find(name); it != loaded.end())
  {
    return it->second;
  }
  // Font not found in loaded fonts! This could happen on first render.
  try 
  {
    auto gui = _gw->GetGUIManager();
    auto font = gui.GetFont(name);
    if (font) loaded.insert(std::pair<string, ImFont*>(name, font));
    return font;
  }
  catch (std::exception& e) 
  {
    LOG("Exeption in {}: {}", __FUNCTION__, e.what());
    // Return default font.
    return _gw->GetGUIManager().GetFont("default");
  }
}
