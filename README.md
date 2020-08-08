# Lime2D File Dialog
A C++ file dialog using Dear ImGui.

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

![L2DFileDialogGif](/screenshots/choose_folder.gif)

### License
This project is under the [Apache License](./license.md).
