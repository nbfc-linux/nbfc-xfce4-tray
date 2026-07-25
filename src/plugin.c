#include "plugin.h"

#include <stdint.h>
#include <libxfce4panel/xfce-panel-plugin.h>

#include "config.h"
#include "nxjson.c"
#include "service_control.c"

#define NBFC_TRAY_MAX_FANS   256
#define NBFC_TRAY_ICON_FILE  DATA_DIR "/tray.ico"

static XfcePanelPlugin* NBFC_Tray_Plugin;
static GtkWidget*       NBFC_Tray_Popup;
static struct FanData   NBFC_Tray_Fans[NBFC_TRAY_MAX_FANS];
static char             NBFC_Tray_SocketPath[4096];

static void Fan_SliderChanged(GtkRange* range, gpointer user_data) {
  const int   fan = (int) (uintptr_t) user_data;
  const float val = gtk_range_get_value(range);

  SetFanSpeed(NBFC_Tray_SocketPath, fan, val);
  gtk_toggle_button_set_active(NBFC_Tray_Fans[fan].toggle, false);
}

static void Fan_ToggleChanged(GtkToggleButton* button, gpointer user_data) {
  const int   fan    = (int) (uintptr_t) user_data;
  const bool  active = gtk_toggle_button_get_active(button);
  const float speed  = active ? -1.0f :
    gtk_range_get_value(GTK_RANGE(NBFC_Tray_Fans[fan].slider));

  SetFanSpeed(NBFC_Tray_SocketPath, fan, speed);
}

static GtkWidget* Fan_CreateControls(int fan_index, float speed, bool auto_mode)
{
  GtkWidget* box;
  GtkWidget* slider;
  GtkWidget* check;
  const gpointer user_data = (gpointer) (uintptr_t) fan_index;

  box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);

  slider = gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL, 0, 100, 1);
  gtk_range_set_value(GTK_RANGE(slider), speed);
  gtk_scale_set_draw_value(GTK_SCALE(slider), TRUE);
  gtk_scale_set_value_pos(GTK_SCALE(slider), GTK_POS_TOP);
  gtk_widget_set_size_request(slider, 80, 200);

  check = gtk_check_button_new_with_label("Auto");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), auto_mode);

  gtk_box_pack_start(GTK_BOX(box), slider, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(box), check, FALSE, FALSE, 0);

  g_signal_connect(
      slider,
      "value-changed",
      G_CALLBACK(Fan_SliderChanged),
      user_data
  );

  g_signal_connect(
      check,
      "toggled",
      G_CALLBACK(Fan_ToggleChanged),
      user_data
  );

  NBFC_Tray_Fans[fan_index].toggle = GTK_TOGGLE_BUTTON(check);
  NBFC_Tray_Fans[fan_index].slider = GTK_SCALE(slider);

  return box;
}

static int NBFC_Tray_GetSocketPath(void) {
  if (NBFC_Tray_SocketPath[0])
    return 0;

  FILE* fh = popen("nbfc show-variable socket_file", "r");
  if (! fh)
    return -1;

  fgets(NBFC_Tray_SocketPath, sizeof(NBFC_Tray_SocketPath), fh);
  NBFC_Tray_SocketPath[strcspn(NBFC_Tray_SocketPath, "\n")] = '\0';
  pclose(fh);

  return NBFC_Tray_SocketPath[0] ? 0 : -1;
}

static GtkWidget* NBFC_Tray_CreatePopup(void)
{
  GtkWidget* box;
  GtkWidget* window = NULL;
  char* buf = NULL;
  const nx_json* response = NULL;
  
  if (NBFC_Tray_GetSocketPath() == -1)
    goto end;

  if (ServiceStatus(NBFC_Tray_SocketPath, &response, &buf) != 0)
    goto end;

  const nx_json* fans = nx_json_get(response, "Fans");
  if (! fans)
    goto end;

  window = gtk_window_new(GTK_WINDOW_POPUP);
  gtk_window_set_type_hint(GTK_WINDOW(window), GDK_WINDOW_TYPE_HINT_POPUP_MENU);
  gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

  box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
  gtk_container_set_border_width(GTK_CONTAINER(box), 10);

  int fan_index = 0;
  for (const nx_json* fan = fans->val.children.first; fan; fan = fan->next) {
    float speed = 100.0f;
    bool auto_mode = false;

    const nx_json* requested_speed_json = nx_json_get(fan, "RequestedSpeed");
    if (requested_speed_json && requested_speed_json->type == NX_JSON_DOUBLE)
      speed = requested_speed_json->val.dbl;

    const nx_json* auto_mode_json = nx_json_get(fan, "AutoMode");
    if (auto_mode_json && auto_mode_json->type == NX_JSON_BOOL)
      auto_mode = auto_mode_json->val.u;

    gtk_box_pack_start(
        GTK_BOX(box),
        Fan_CreateControls(fan_index, speed, auto_mode),
        TRUE, TRUE, 0
    );

    ++fan_index;
  }

  gtk_container_add(GTK_CONTAINER(window), box);

end:
  free(buf);
  nx_json_free(response);
  return window;
}

static gboolean NBFC_Tray_PopupFocusOut(GtkWidget* widget, GdkEventFocus* event, gpointer _data)
{
  if (NBFC_Tray_Popup != NULL) {
    gtk_widget_destroy(NBFC_Tray_Popup);
    NBFC_Tray_Popup = NULL;
  }

  return FALSE;
}

static void NBFC_Tray_PopupDestroyed(GtkWidget* widget, gpointer _data) {
  NBFC_Tray_Popup = NULL;
}

static gboolean NBFC_Tray_ButtonPress(GtkWidget* widget, GdkEventButton* event, gpointer _data)
{
  if (event->button != 1)
    return FALSE;

  // Close the popup if it's open
  if (NBFC_Tray_Popup != NULL) {
    gtk_widget_destroy(NBFC_Tray_Popup);
    NBFC_Tray_Popup = NULL;
    return TRUE;
  }

  // Create new popup
  NBFC_Tray_Popup = NBFC_Tray_CreatePopup();
  if (! NBFC_Tray_Popup)
    return TRUE;

  g_signal_connect(
      NBFC_Tray_Popup,
      "focus-out-event",
      G_CALLBACK(NBFC_Tray_PopupFocusOut),
      NULL
  );

  g_signal_connect(
      NBFC_Tray_Popup,
      "destroy",
      G_CALLBACK(NBFC_Tray_PopupDestroyed),
      NULL
  );

  // IMPORTANT:
  // XFCE positions an invisible window.
  // Show it afterwards.
  xfce_panel_plugin_popup_window(NBFC_Tray_Plugin, GTK_WINDOW(NBFC_Tray_Popup), widget);
  gtk_widget_show_all(NBFC_Tray_Popup);
  gtk_widget_grab_focus(NBFC_Tray_Popup);
  return TRUE;
}

static GtkWidget* NBFC_Tray_GetIcon(void) {
  gint width, height;
  gtk_icon_size_lookup(GTK_ICON_SIZE_BUTTON, &width, &height);

  GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file_at_size(
      NBFC_TRAY_ICON_FILE, width, height, NULL);

  GtkWidget* image = gtk_image_new_from_pixbuf(pixbuf);
  g_object_unref(pixbuf);

  return image;
}

void NBFC_Tray_Construct(XfcePanelPlugin* plugin)
{
  NBFC_Tray_Plugin     = plugin;
  GtkWidget* event_box = gtk_event_box_new();
  GtkWidget* image     = NBFC_Tray_GetIcon();

  gtk_container_add(GTK_CONTAINER(event_box), image);
  gtk_container_add(GTK_CONTAINER(plugin), event_box);

  g_signal_connect(
      event_box,
      "button-press-event",
      G_CALLBACK(NBFC_Tray_ButtonPress),
      NULL
  );

  gtk_widget_show_all(GTK_WIDGET(NBFC_Tray_Plugin));
}

XFCE_PANEL_PLUGIN_REGISTER(NBFC_Tray_Construct)
