#include "TextureBrowser.hpp"
#include "TextureManager.hpp"
#include <ImGuiFileDialog.h>
#include <filesystem>
#include <fstream>
#include <imgui.h>

void
copyToAssetFolder(const std::string& srcPath, const std::string& destPath)
{
  if (!std::filesystem::exists(srcPath))
  {
    return;
  }
  std::filesystem::create_directories(std::filesystem::path(destPath).parent_path());
  std::ifstream src(srcPath, std::ios::binary);
  std::ofstream dst(destPath, std::ios::binary);

  if (!src || !dst)
  {
    return;
  }

  dst << src.rdbuf();
}

TextureAssetBrowser::TextureAssetBrowser(TextureManager*    textureManager,
                                         const std::string& assetDir)
    : _textureManager(textureManager), _assetDir(assetDir)
{
}

void
TextureAssetBrowser::loadTextures()
{
  namespace fs = std::filesystem;
  for (const auto& file : fs::directory_iterator(_assetDir))
  {
    if (!file.is_regular_file())
      continue;

    auto path = file.path();
    if (path.extension() != ".png")
      continue;

    std::string id   = path.stem().string();
    std::string full = path.string();

    if (!_textureManager->hasTexture(id))
    {
      _textureManager->addTexture(id, full);
      _textureIDs.push_back(id);
    }
  }
}

void
TextureAssetBrowser::draw()
{
  if (!_loaded)
  {
    loadTextures();
    _loaded = true;
  }

  if (ImGui::Button("+ Add Texture"))
  {
    ImGuiFileDialog::Instance()->OpenDialog("ChooseTex", "Choose Texture", ".png");
  }

  if (ImGuiFileDialog::Instance()->Display("ChooseTex"))
  {
    if (ImGuiFileDialog::Instance()->IsOk())
    {
      std::string fullPath = ImGuiFileDialog::Instance()->GetFilePathName();
      std::string filename = ImGuiFileDialog::Instance()->GetCurrentFileName();
      std::string id       = std::filesystem::path(filename).stem().string();

      std::string destPath = _assetDir + "/" + filename;

      copyToAssetFolder(fullPath, destPath);

      if (!_textureManager->hasTexture(id))
      {
        if (_textureManager->addTexture(id, fullPath))

          _textureIDs.push_back(id);
      }
    }
    ImGuiFileDialog::Instance()->Close();
  }

  float padding    = 8.f;
  float cellSize   = _thumbSize + padding;
  float panelWidth = ImGui::GetContentRegionAvail().x;
  int   columns    = (int) (panelWidth / cellSize);
  if (columns < 1)
    columns = 1;

  ImGui::Columns(columns, nullptr, false);

  for (int i = 0; i < (int) _textureIDs.size();)
  {
    ImGui::PushID(i);
    const std::string& texID = _textureIDs[i];

    if (ImGui::Button("png", ImVec2(_thumbSize, _thumbSize)))
      _selectedIndex = i;

    if (ImGui::BeginPopupContextItem())
    {
      _contextIndex = i;
      if (ImGui::Selectable("Rename"))
      {
        _renameIndex = i;
        strncpy(_renameBuffer, texID.c_str(), sizeof(_renameBuffer));
      }
      if (ImGui::Selectable("Delete"))
      {

        std::string filePath = _assetDir + "/" + texID + ".png";
        if (std::filesystem::exists(filePath))
          std::filesystem::remove(filePath);

        _textureManager->unloadTexture(texID);
        _textureIDs.erase(_textureIDs.begin() + i);
        if (_selectedIndex == i)
          _selectedIndex = -1;
        ImGui::EndPopup();
        ImGui::PopID();
        continue;
      }
      ImGui::EndPopup();
    }

    if (_renameIndex == i)
    {
      if (ImGui::InputText("##Rename",
                           _renameBuffer,
                           sizeof(_renameBuffer),
                           ImGuiInputTextFlags_EnterReturnsTrue))
      {
        std::string newID = _renameBuffer;
        if (newID != texID && !_textureManager->hasTexture(newID))
        {

          std::string oldPath = _assetDir + "/" + texID + ".png";
          std::string newPath = _assetDir + "/" + newID + ".png";

          if (std::filesystem::exists(oldPath))
          {
            std::filesystem::rename(oldPath, newPath);
            _textureManager->renameTexture(texID, newID);
            _textureIDs[i] = newID;
          }
        }
        _renameIndex = -1;
      }

      if (ImGui::IsKeyPressed(ImGuiKey_Escape))
        _renameIndex = -1;
    }
    else
    {
      ImGui::TextWrapped("%s", texID.c_str());
    }

    ImGui::NextColumn();
    ImGui::PopID();
    ++i;
  }

  ImGui::Columns(1);
}
