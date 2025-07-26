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

TextureBrowser::TextureBrowser(TextureManager* textureManager, const std::string& assetDir)
    : _textureManager(textureManager), _assetDir(assetDir)
{
  loadTextures();
  _loaded = true;
}

void
TextureBrowser::loadTextures()
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
TextureBrowser::draw()
{

  if (!_loaded)
  {
    loadTextures();
    _loaded = true;
  }

  ImGui::Begin("TEXTURES");

  float padding      = 8.f;
  float cellSize     = _thumbSize + padding;
  float splitWidth   = ImGui::GetContentRegionAvail().x;
  float gridWidth    = splitWidth * 0.3f;
  float previewWidth = splitWidth - gridWidth - 8.f;

  ImGui::BeginChild("TextureGrid", ImVec2(gridWidth, 0), true);
  if (ImGui::Button("Import"))
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
        if (_textureManager->addTexture(id, destPath))
          _textureIDs.push_back(id);
      }
    }
    ImGuiFileDialog::Instance()->Close();
  }

  ImGui::Separator();
  int columns = std::max(1, int(gridWidth / cellSize));
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
  ImGui::EndChild();
  ImGui::SameLine();
  ImGui::BeginChild("TexturePreview", ImVec2(0, 0), true);

  if (_selectedIndex >= 0 && _selectedIndex < (int) _textureIDs.size())
  {
    const std::string& selectedID = _textureIDs[_selectedIndex];
    auto*              tex        = _textureManager->getTexture(selectedID);
    if (tex)
    {
      int w, h;
      SDL_QueryTexture(tex, nullptr, nullptr, &w, &h);

      float  maxPreviewSize = previewWidth - 16.f;
      float  scale          = std::min(maxPreviewSize / w, maxPreviewSize / h);
      ImVec2 scaledSize(w * scale, h * scale);

      ImGui::Text("Preview: %s", selectedID.c_str());
      ImGui::Text("Size: %dx%d", w, h);
      ImGui::Image(reinterpret_cast<ImTextureID>(tex), scaledSize);
    }
    else
    {
      ImGui::Text("Missing texture.");
    }
  }

  ImGui::EndChild();
  ImGui::End();
}
