static const char gladestring [] = 
{
  "<?xml version=\"1.0\" ?><!-- Generated with glade 3.18.3 --><interface><requires lib=\"gtk+\" ve"
  "rsion=\"3.12\"/><!-- interface-license-type gplv3 --><!-- interface-name Gtk3Run --><!-- interfa"
  "ce-description Gtk3 Run Command --><!-- interface-authors J. M. Becker <cyberpunkspike@gmail.com"
  "> --><object class=\"GtkListStore\" id=\"liststore1\"><columns><!-- column-name COL_NAME --><col"
  "umn type=\"gchararray\"/><!-- column-name COL_FLAG --><column type=\"gint\"/></columns></object>"
  "<object class=\"GtkEntryCompletion\" id=\"entrycompletion1\"><property name=\"model\">liststore1"
  "</property><property name=\"text_column\">0</property><property name=\"popup_set_width\">False</"
  "property><property name=\"popup_single_match\">False</property><signal handler=\"cb_matchselecte"
  "d\" name=\"match-selected\" swapped=\"no\"/><child><object class=\"GtkCellRendererText\" id=\"te"
  "xtCellRenderer\"><property name=\"ypad\">1</property></object><attributes><attribute name=\"text"
  "\">0</attribute></attributes></child></object><object class=\"GtkWindow\" id=\"window1\"><proper"
  "ty name=\"can_focus\">False</property><property name=\"title\" translatable=\"yes\">Run Command<"
  "/property><property name=\"resizable\">False</property><property name=\"window_position\">center"
  "</property><property name=\"type_hint\">dialog</property><property name=\"decorated\">False</pro"
  "perty><signal handler=\"cb_destroy\" name=\"destroy\" swapped=\"no\"/><child><object class=\"Gtk"
  "Box\" id=\"box1\"><property name=\"visible\">True</property><property name=\"can_focus\">False</"
  "property><property name=\"orientation\">vertical</property><child><object class=\"GtkEntry\" id="
  "\"entry1\"><property name=\"visible\">True</property><property name=\"can_focus\">True</property"
  "><property name=\"margin_left\">4</property><property name=\"margin_right\">4</property><propert"
  "y name=\"margin_top\">4</property><property name=\"width_chars\">60</property><property name=\"c"
  "aps_lock_warning\">False</property><property name=\"primary_icon_activatable\">False</property><"
  "property name=\"secondary_icon_activatable\">False</property><property name=\"primary_icon_sensi"
  "tive\">False</property><property name=\"secondary_icon_sensitive\">False</property><property nam"
  "e=\"placeholder_text\" translatable=\"yes\">Run Command:</property><property name=\"completion\""
  ">entrycompletion1</property><property name=\"input_hints\">GTK_INPUT_HINT_WORD_COMPLETION | GTK_"
  "INPUT_HINT_NONE</property><signal handler=\"cb_activate\" name=\"activate\" object=\"statusbar1\""
  " swapped=\"no\"/><signal handler=\"cb_deletetext\" name=\"delete-text\" object=\"liststore1\" sw"
  "apped=\"no\"/><signal handler=\"cb_inserttext\" name=\"insert-text\" object=\"liststore1\" swapp"
  "ed=\"no\"/></object><packing><property name=\"expand\">False</property><property name=\"fill\">T"
  "rue</property><property name=\"position\">1</property></packing></child><child><object class=\"G"
  "tkStatusbar\" id=\"statusbar1\"><property name=\"visible\">True</property><property name=\"can_f"
  "ocus\">False</property><property name=\"margin_left\">12</property><property name=\"margin_right"
  "\">12</property><property name=\"margin_start\">10</property><property name=\"margin_end\">10</p"
  "roperty><property name=\"orientation\">vertical</property><signal handler=\"cb_textpushed\" name"
  "=\"text-pushed\" swapped=\"no\"/></object><packing><property name=\"expand\">False</property><pr"
  "operty name=\"fill\">True</property><property name=\"position\">2</property></packing></child></"
  "object></child></object></interface>"
};
