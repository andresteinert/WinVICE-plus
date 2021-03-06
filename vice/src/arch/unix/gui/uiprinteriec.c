/*
 * uiprinteriec.c
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *
 * This file is part of VICE, the Versatile Commodore Emulator.
 * See README for copyright notice.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA.
 *
 */

#include "vice.h"

#include <stdio.h>
#include <string.h>

#include "printer.h"
#include "uimenu.h"
#include "uiprinter.h"

UI_MENU_DEFINE_TOGGLE(PrinterUserport)
UI_MENU_DEFINE_TOGGLE(IECDevice4)
UI_MENU_DEFINE_TOGGLE(IECDevice5)
UI_MENU_DEFINE_TOGGLE(IECDevice6)

#ifdef HAVE_OPENCBM
UI_MENU_DEFINE_TOGGLE(IECDevice7)
UI_MENU_DEFINE_TOGGLE(Printer7)
#endif

static ui_menu_entry_t printeriec_userport_menu_items[] = {
    UI_MENU_ENTRY_SEPERATOR,
    { N_("Userport printer emulation"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_PrinterUserport, NULL, NULL,
      (ui_keysym_t)0, (ui_hotkey_modifier_t)0 },
    { N_("Userport printer driver"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, uiprinter_pruser_driver_submenu,
      (ui_keysym_t)0, (ui_hotkey_modifier_t)0 },
    { N_("Userport printer output"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, uiprinter_prUserport_output_submenu,
      (ui_keysym_t)0, (ui_hotkey_modifier_t)0 },
    { N_("Userport printer text output device"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, uiprinter_pruser_device_submenu,
      (ui_keysym_t)0, (ui_hotkey_modifier_t)0 },
    { N_("Userport printer formfeed"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)uiprinter_formfeed, (ui_callback_data_t)3, NULL,
      (ui_keysym_t)0, (ui_hotkey_modifier_t)0 },
    UI_MENU_ENTRY_SEPERATOR,
    UI_MENU_ENTRY_LIST_END
};

static ui_menu_entry_t printeriec_userport_menu[] = {
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, printeriec_userport_menu_items,
      (ui_keysym_t)0, (ui_hotkey_modifier_t)0 },
    UI_MENU_ENTRY_LIST_END
};

static ui_menu_entry_t printeriec_no_userport_menu[] = {
    UI_MENU_ENTRY_SEPERATOR,
    UI_MENU_ENTRY_LIST_END
};

#define PRINTERIEC_SETTINGS_MENU_COMMON(x, y, z)                      \
    { N_("Printer #"#x" emulation"), UI_MENU_TYPE_NORMAL,             \
      NULL, NULL, uiprinter_set_printer##x##_type_submenu,            \
      (ui_keysym_t)0, (ui_hotkey_modifier_t)0 },                      \
    { N_("Printer #"#x" enable IEC device"), UI_MENU_TYPE_TICK,       \
      (ui_callback_t)toggle_IECDevice##x, NULL, NULL,                 \
      (ui_keysym_t)0, (ui_hotkey_modifier_t)0 },                      \
    { N_("Printer #"#x" driver"), UI_MENU_TYPE_NORMAL,                \
      NULL, NULL, uiprinter_pr##x##_driver_submenu,                   \
      (ui_keysym_t)0, (ui_hotkey_modifier_t)0 },                      \
    { N_("Printer #"#x" output"), UI_MENU_TYPE_NORMAL,                \
      NULL, NULL, uiprinter_pr##x##_output_submenu,                   \
      (ui_keysym_t)0, (ui_hotkey_modifier_t)0 },                      \
    { N_("Printer #"#x" text output device"), UI_MENU_TYPE_NORMAL,    \
      NULL, NULL, uiprinter_pr##x##_device_submenu,                   \
      (ui_keysym_t)0, (ui_hotkey_modifier_t)0 },                      \
    { N_("Printer #"#x" formfeed"), UI_MENU_TYPE_NORMAL,              \
      (ui_callback_t)uiprinter_formfeed, (ui_callback_data_t)y, NULL, \
      z, UI_HOTMOD_META }

ui_menu_entry_t printeriec_settings_menu[] = {
    PRINTERIEC_SETTINGS_MENU_COMMON(4, 0, KEYSYM_4),
    UI_MENU_ENTRY_SEPERATOR,
    PRINTERIEC_SETTINGS_MENU_COMMON(5, 1, KEYSYM_5),
    UI_MENU_ENTRY_SEPERATOR,
    PRINTERIEC_SETTINGS_MENU_COMMON(6, 1, KEYSYM_6),
#ifdef HAVE_OPENCBM
    UI_MENU_ENTRY_SEPERATOR,
    { N_("Device #7 Real device access"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_Printer7, (ui_callback_data_t)PRINTER_DEVICE_REAL, NULL,
      (ui_keysym_t)0, (ui_hotkey_modifier_t)0 },
    { N_("Device #7 enable IEC device"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_IECDevice7, NULL, NULL,
      (ui_keysym_t)0, (ui_hotkey_modifier_t)0 },
#endif
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, printeriec_userport_menu_items,
      (ui_keysym_t)0, (ui_hotkey_modifier_t)0 },
    { N_("Printer text device 1"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)uiprinter_set_printer_exec_file, (ui_callback_data_t)"PrinterTextDevice1", NULL,
      (ui_keysym_t)0, (ui_hotkey_modifier_t)0 },
    { N_("Printer text device 2"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)uiprinter_set_printer_exec_file, (ui_callback_data_t)"PrinterTextDevice2", NULL,
      (ui_keysym_t)0, (ui_hotkey_modifier_t)0 },
    { N_("Printer text device 3"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)uiprinter_set_printer_exec_file, (ui_callback_data_t)"PrinterTextDevice3", NULL,
      (ui_keysym_t)0, (ui_hotkey_modifier_t)0 },
    UI_MENU_ENTRY_LIST_END
};

void uiprinter_pruser_enable(int enable)
{
    memcpy(
#ifdef HAVE_OPENCBM
        &printeriec_settings_menu[23],
#else
        &printeriec_settings_menu[20],
#endif
        (enable ? &printeriec_userport_menu[0] : &printeriec_no_userport_menu[0]),
        sizeof(ui_menu_entry_t));
}
