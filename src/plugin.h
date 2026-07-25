#ifndef NBFC_TRAY_PLUGIN_H_
#define NBFC_TRAY_PLUGIN_H_

#include <gtk/gtk.h>
#include <libxfce4panel/xfce-panel-plugin.h>
#include <libxfce4panel/xfce-panel-macros.h>

struct FanData {
  GtkScale* slider;
  GtkToggleButton* toggle;
};

G_BEGIN_DECLS

void NBFC_Tray_Construct(XfcePanelPlugin* plugin);

G_END_DECLS

#endif
