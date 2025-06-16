//
// Created by 冯宇飞 on 25-6-14.
//

#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <string>

#include "core/audio_file.h"
#include "FL/Fl_Widget.H"

class Callbacks {
};

void show_choose_base_dir(Fl_Widget *, void *);

void search_callback(Fl_Widget *, void *);

void to_file_list_callback(Fl_Widget *, void *);

void import_callback(Fl_Widget *, void *);

void export_callback(Fl_Widget *, void *);

void show_choose_replace_base_dir(Fl_Widget *, void *);

void save_browser_selected_items();

void insert_to_audio_files(const std::string & path,bool is_selected);

#endif //CALLBACKS_H
