# Lime2D File Dialog
A C++ file dialog using Dear ImGui.

<a href='https://ko-fi.com/Q5Q8FGABI' target='_blank'><img height='36' style='border:0px;height:36px;' src='https://cdn.ko-fi.com/cdn/kofi2.png?v=3' border='0' alt='Buy Me a Coffee at ko-fi.com' /></a>

![L2DFileDialog](/screenshots/main_pic.png?raw=true "L2DFileDialog")

### Usage
Add `L2DFileDialog.h` to your C++ project and include it where you use ImGui. Then, set `FileDialog::fileDialogOpen` to `true` to set the open state. Finally, in your `update` method, check if `FileDialog::fileDialogOpen == true` and if so, call `FileDialog::ShowFileDialog`, passing in a `char` array as a buffer to store the chosen file/folder path.

### Features
* New folder
* Delete folder
* Sort by:
  * File
  * Size
  * Type
  * Last modified date

### Example
You can see `L2dFileDialog/src/main.cpp` for a full example. Here is a very minimal sample:
```c++
...
static char* file_dialog_buffer = nullptr;
static char path[500] = "";

ImGui::TextUnformatted("Path: ");
ImGui::InputText("##path", path, sizeof(path));
ImGui::SameLine();
if (ImGui::Button("Browse##path")) {
  file_dialog_buffer = path;
  FileDialog::file_dialog_open = true;
  FileDialog::file_dialog_open_type = FileDialog::FileDialogType::SelectFolder;
}

if (FileDialog::file_dialog_open) {
  FileDialog::ShowFileDialog(&FileDialog::file_dialog_open, file_dialog_buffer, sizeof(file_dialog_buffer), FileDialog::file_dialog_open_type);
}
...
```

![L2DFileDialogGif](/screenshots/choose_folder.gif)

### License
This project is under the [Apache License](./license.md).
