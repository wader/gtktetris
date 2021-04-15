/*
 * This is free and unencumbered software released into the public domain.
 *
 * For more information, please refer to <https://unlicense.org>
 */

/** 2021-02-07 **/

/*
 * gtkcompat.h, GTK2+ compatibility layer
 * 
 * This lib makes it easier to support older GTK versions
 * while still avoiding deprecated functions as much as possible.
 * 
 * The older the GTK version, the more compatible functions are "defined"
 * so it's not wise to use the compiled binary in newer distros or something.
 * 
 * Apps should support gtk2 >= 2.14 / gtk3 >= 3.14
 * 
 */

/* 
special defines:
	gtkcompat_widget_set_halign_left   (w)
	gtkcompat_widget_set_halign_center (w)
	gtkcompat_widget_set_halign_right  (w)
*/

/*
GTKCOMPAT_DRAW_SIGNAL (gtk3="draw", gtk2="expose_event")
---------------------
 g_signal_connect (w, GTKCOMPAT_DRAW_SIGNAL, G_CALLBACK (w_draw_cb), NULL);
 gboolean w_draw_cb (GtkWidget *w, gpointer compat, gpointer user_data)
 {
 #if GTK_CHECK_VERSION (3, 0, 0)
    cairo_t * cr = (cairo_t *) compat;
 #else // gtk2
    //GdkEventExpose * event = (GdkEventExpose *) compat;
    cairo_t * cr = gdk_cairo_create (gtk_widget_get_window (w));
 #endif
 #if GTK_MAJOR_VERSION == 2
    cairo_destroy (cr);
 #endif
 }
*/

#ifndef __GTKCOMPAT_H
#define __GTKCOMPAT_H

#ifdef __cplusplus
extern "C"
{
#endif


#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#if GTK_MAJOR_VERSION == 3
#include <gdk/gdkkeysyms-compat.h>
#endif


/* ================================================== */
/*                      GLIB                          */
/* ================================================== */

#ifndef __GLIB_COMPAT_H
#define __GLIB_COMPAT_H

#include <glib.h>

// GLIB < 2.58
#if ! GLIB_CHECK_VERSION (2, 58, 0)
#define G_SOURCE_FUNC(f) ((GSourceFunc) (void (*)(void)) (f))
#endif


// GLIB < 2.40
#if ! GLIB_CHECK_VERSION (2, 40, 0)
#define g_key_file_save_to_file(kfile,filename,error) { \
   char * data = g_key_file_to_data (kfile, NULL, error); \
   g_file_set_contents (filename, data, -1, error); \
   g_free (data); \
}
#endif


// GLIB < 2.32
#if ! GLIB_CHECK_VERSION (2, 32, 0)
#define G_SOURCE_REMOVE   FALSE
#define G_SOURCE_CONTINUE TRUE
#define GRecMutex              GStaticRecMutex
#define g_rec_mutex_init(x)    g_static_rec_mutex_init(x)
#define g_rec_mutex_lock(x)    g_static_rec_mutex_lock(x)
#define g_rec_mutex_trylock(x) g_static_rec_mutex_trylock(x)
#define g_rec_mutex_unlock(x)  g_static_rec_mutex_unlock(x)
#define g_rec_mutex_clear(x)   g_static_rec_mutex_free(x)
#define g_thread_new(name,func,data) g_thread_create(func,data,TRUE,NULL)
#define g_thread_try_new(name,func,data,error) g_thread_create(func,data,TRUE,error)
#define g_hash_table_add(ht,key)      g_hash_table_replace(ht,key,key)
#define g_hash_table_contains(ht,key) g_hash_table_lookup_extended(ht,key,NULL,NULL)
#endif


// GMutex vs GStaticMutex
#if GLIB_CHECK_VERSION (2, 32, 0)
// since version 2.32.0 GMutex can be statically allocated
// don't use WGMutex to replace GMutex * ... issues, errors.
#	define WGMutex GMutex
#	define Wg_mutex_init    g_mutex_init
#	define Wg_mutex_lock    g_mutex_lock
#	define Wg_mutex_trylock g_mutex_trylock
#	define Wg_mutex_unlock  g_mutex_unlock
#	define Wg_mutex_clear   g_mutex_clear
#else
#	define WGMutex GStaticMutex
#	define Wg_mutex_init    g_static_mutex_init
#	define Wg_mutex_lock    g_static_mutex_lock
#	define Wg_mutex_trylock g_static_mutex_trylock
#	define Wg_mutex_unlock  g_static_mutex_unlock
#	define Wg_mutex_clear   g_static_mutex_free
// sed -i 's%g_mutex_%Wg_mutex_%g' $(grep "g_mutex_" *.c *.h | cut -f 1 -d ":" | grep -v -E 'gtkcompat|glib-compat' | sort -u)
#endif


// GLIB < 2.30
#if ! GLIB_CHECK_VERSION (2, 30, 0)
#define g_format_size g_format_size_for_display
#endif


// GLIB < 2.28
#if ! GLIB_CHECK_VERSION (2, 28, 0)
#define g_list_free_full(list,free_func) {\
     g_list_foreach (list, (GFunc) free_func, NULL);\
     g_list_free (list);\
}
#endif


// GLIB < 2.22
#if ! GLIB_CHECK_VERSION (2, 22, 0)
#define g_mapped_file_unref(x) g_mapped_file_free(x)
#endif

// GLIB < 2.20
#if ! GLIB_CHECK_VERSION (2, 20, 0)
#define g_app_info_get_commandline(app) g_app_info_get_executable(app)
#endif

/* glib 2.18+ tested */

#endif /* __GLIB_COMPAT_H */



/* ================================================== */
/*                       GTK                          */
/* ================================================== */

// STOCK items work in GTK2/3 but not in GTK4
#if GTK_MAJOR_VERSION >= 4
#define GTKCOMPAT_STOCK_OK     "_OK"
#define GTKCOMPAT_STOCK_CANCEL "_Cancel"
#define GTKCOMPAT_STOCK_APPLY  "_Apply"
#define GTKCOMPAT_STOCK_CLOSE  "_Close"
#define GTKCOMPAT_STOCK_YES    "_Yes"
#define GTKCOMPAT_STOCK_NO     "_No"
#define GTKCOMPAT_STOCK_SAVE   "_Save"
#define GTKCOMPAT_STOCK_QUIT   "_Quit"
#define GTKCOMPAT_STOCK_OPEN   "_Open"
#define GTKCOMPAT_STOCK_ABOUT  "_About"
#else
#define GTKCOMPAT_STOCK_OK     "gtk-ok"
#define GTKCOMPAT_STOCK_CANCEL "gtk-cancel"
#define GTKCOMPAT_STOCK_APPLY  "gtk-apply"
#define GTKCOMPAT_STOCK_CLOSE  "gtk-close"
#define GTKCOMPAT_STOCK_YES    "gtk-yes"
#define GTKCOMPAT_STOCK_NO     "gtk-no"
#define GTKCOMPAT_STOCK_SAVE   "gtk-save"
#define GTKCOMPAT_STOCK_QUIT   "gtk-quit"
#define GTKCOMPAT_STOCK_OPEN   "gtk-open"
#define GTKCOMPAT_STOCK_ABOUT  "gtk-about"
#endif


/* ================================================== */
/*                       GTK 3                        */
/* ================================================== */

// GTK >= 3.0 -- applies to GTK3, GTK4...
#if GTK_MAJOR_VERSION >= 3
#define GTKCOMPAT_DRAW_SIGNAL "draw"
#define gtkcompat_widget_set_halign_left(w)   gtk_widget_set_halign(GTK_WIDGET(w), GTK_ALIGN_START)
#define gtkcompat_widget_set_halign_center(w) gtk_widget_set_halign(GTK_WIDGET(w), GTK_ALIGN_CENTER)
#define gtkcompat_widget_set_halign_right(w)  gtk_widget_set_halign(GTK_WIDGET(w), GTK_ALIGN_END)
#endif


// GTK < 3.12
#if ! GTK_CHECK_VERSION (3, 12, 0)
#define gtk_application_set_accels_for_action(app,name,accels) \
          gtk_application_add_accelerator(app,accels[0],name,NULL)
#define gtk_widget_set_margin_start(widget,margin) gtk_widget_set_margin_left(widget,margin)
#define gtk_widget_set_margin_end(widget,margin)   gtk_widget_set_margin_right(widget,margin)
#endif


// GTK < 3.10
#if GTK_CHECK_VERSION (3, 10, 0)
// gdk_window_create_similar_image_surface() was removed in gtk4
//                  only use gdk_window_create_similar_surface()
#endif


// GTK < 3.8
#if ! GTK_CHECK_VERSION (3, 8, 0)
#define gtk_widget_set_opacity(w,o) gtk_window_set_opacity(GTK_WINDOW(w),o)
#define gtk_widget_get_opacity(w)  (gtk_window_get_opacity(GTK_WINDOW(w))
#endif


// GTK < 3.4
#if ! GTK_CHECK_VERSION (3, 4, 0)
#define gtk_application_window_new(app) gtk_window_new(GTK_WINDOW_TOPLEVEL)
#endif



/* ================================================== */
/*                       GTK 2                        */
/* ================================================== */

// define some GTK3+ functions
#if GTK_MAJOR_VERSION <= 2
#define GTKCOMPAT_DRAW_SIGNAL "expose_event"
#define gtk_box_new(ori,spacing) \
  ((ori == GTK_ORIENTATION_HORIZONTAL) ? gtk_hbox_new(FALSE,spacing) \
                                       : gtk_vbox_new(FALSE,spacing))
#define gtk_button_box_new(ori) \
  ((ori == GTK_ORIENTATION_HORIZONTAL) ? gtk_hbutton_box_new() \
                                       : gtk_vbutton_box_new())
#define gtk_scale_new(ori,adjustment) \
  ((ori == GTK_ORIENTATION_HORIZONTAL) ? gtk_hscale_new(adjustment) \
                                       : gtk_vscale_new(adjustment))
#define gtk_scale_new_with_range(ori,min,max,step) \
  ((ori == GTK_ORIENTATION_HORIZONTAL) ? gtk_hscale_new_with_range(min,max,step) \
                                       : gtk_vscale_new_with_range(min,max,step))
#define gtk_separator_new(ori) \
  ((ori == GTK_ORIENTATION_HORIZONTAL) ? gtk_hseparator_new() \
                                       : gtk_vseparator_new())
#define gtk_scrollbar_new(ori,adjustment) \
  ((ori == GTK_ORIENTATION_HORIZONTAL) ? gtk_hscrollbar_new(adjustment) \
                                       : gtk_vscrollbar_new(adjustment))
#define gtk_paned_new(ori) \
  ((ori == GTK_ORIENTATION_HORIZONTAL) ? gtk_hpaned_new() \
                                       : gtk_vpaned_new())
#define gtk_widget_get_allocated_height(widget) (GTK_WIDGET(widget)->allocation.height )
#define gtk_widget_get_allocated_width(widget)  (GTK_WIDGET(widget)->allocation.width  )
#define gtk_combo_box_text_remove_all(cmb) { \
   gtk_list_store_clear (GTK_LIST_STORE (gtk_combo_box_get_model (GTK_COMBO_BOX (cmb)))); \
}
#define gtk_tree_model_iter_previous(model,iter) ({ \
   GtkTreePath * path = gtk_tree_model_get_path (model, iter); \
   gboolean valid = gtk_tree_path_prev (path); \
   if (valid) gtk_tree_model_get_iter (model, iter, path); \
   gtk_tree_path_free (path); \
   valid; \
})
#define gtk_widget_override_font(w,f) gtk_widget_modify_font(w,f)
#define gtkcompat_widget_set_halign_left(w)   gtk_misc_set_alignment(GTK_MISC(w), 0.0, 0.5)
#define gtkcompat_widget_set_halign_center(w) gtk_misc_set_alignment(GTK_MISC(w), 0.5, 0.5)
#define gtkcompat_widget_set_halign_right(w)  gtk_misc_set_alignment(GTK_MISC(w), 1.0, 0.5)
/* GtkApplication */
#define GtkApplication void
#define g_application_quit(app) gtk_main_quit()
#undef G_APPLICATION
#define G_APPLICATION(app) ((void *) (app))
#endif


// GTK < 2.24
#if ! GTK_CHECK_VERSION (2, 24, 0)
typedef struct _GtkComboBox        GtkComboBoxText;
typedef struct _GtkComboBoxClass   GtkComboBoxTextClass;
typedef struct _GtkComboBoxPrivate GtkComboBoxTextPrivate;
#define GTK_TYPE_COMBO_BOX_TEXT            (gtk_combo_box_get_type ())
#define GTK_COMBO_BOX_TEXT(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_COMBO_BOX, GtkComboBoxText))
#define GTK_COMBO_BOX_TEXT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_TYPE_COMBO_BOX, GtkComboBoxTextClass))
#define GTK_IS_COMBO_BOX_TEXT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_COMBO_BOX))
#define GTK_IS_COMBO_BOX_TEXT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_COMBO_BOX))
#define GTK_COMBO_BOX_TEXT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_COMBO_BOX, GtkComboBoxTextClass))
#define gtk_combo_box_text_new() gtk_combo_box_new_text()
#define gtk_combo_box_text_new_with_entry()	gtk_combo_box_entry_new_text()
#define gtk_combo_box_text_append_text(combo,text) gtk_combo_box_append_text(combo,text)
#define gtk_combo_box_text_insert_text(combo,pos,text) gtk_combo_box_insert_text(combo,pos,text)
#define gtk_combo_box_text_prepend_text(combo,text) gtk_combo_box_prepend_text(combo,text)
#define gtk_combo_box_text_remove(combo,pos) gtk_combo_box_remove_text(combo,pos)
#define gtk_combo_box_text_get_active_text(combo) (gtk_combo_box_get_active_text(combo))
#define gtk_combo_box_get_has_entry(combo) (0)
#define gtk_combo_box_set_entry_text_column(combo,cl)
#define gtk_combo_box_get_entry_text_column(combo) (0)
#define gtk_range_get_round_digits(range) (GTK_RANGE(range)->round_digits)
//#define gdk_window_get_visual(w)  (gdk_drawable_get_visual(GDK_DRAWABLE(w)))
#define gdk_window_get_screen(w)  (gdk_drawable_get_screen(GDK_DRAWABLE(w)))
#define gdk_window_get_display(w) (gdk_drawable_get_display(GDK_DRAWABLE(w)))
#define gtk_notebook_get_group_name gtk_notebook_get_group
#define gtk_notebook_set_group_name gtk_notebook_set_group
#endif


// GTK < 2.22
#if ! GTK_CHECK_VERSION (2, 22, 0)
#define gtk_window_has_group(w) (GTK_WINDOW(w)->group != NULL)
#define gtk_window_group_get_current_grab(wg) \
  ((GTK_WINDOW_GROUP(wg)->grabs) ? GTK_WIDGET(GTK_WINDOW_GROUP(wg)->grabs->data) : NULL)
#define gtk_font_selection_dialog_get_font_selection(fsd)(GTK_FONT_SELECTION_DIALOG(fsd)->fontsel)
#define gtk_notebook_get_tab_hborder(n) (GTK_NOTEBOOK(n)->tab_hborder)
#define gtk_notebook_get_tab_vborder(n) (GTK_NOTEBOOK(n)->tab_vborder)
#define gtk_button_get_event_window(button) (GTK_BUTTON(button)->event_window)
#define gdk_visual_get_visual_type(visual) (GDK_VISUAL(visual)->type)
#define gdk_visual_get_depth(visual) (GDK_VISUAL(visual)->depth)
#define gdk_visual_get_byte_order(visual) (GDK_VISUAL(visual)->byte_order)
#define gdk_visual_get_colormap_size(visual) (GDK_VISUAL(visual)->colormap_size)
#define gdk_visual_get_bits_per_rgb(visual) (GDK_VISUAL(visual)->bits_per_rgb)
#define gtk_icon_view_get_item_orientation gtk_icon_view_get_orientation
#define gtk_icon_view_set_item_orientation gtk_icon_view_set_orientation
#define gdk_window_create_similar_surface(gdksurf,content,width,height) ({ \
   cairo_t * wcr = gdk_cairo_create (gdksurf); \
   cairo_surface_t * window_surface = cairo_get_target (wcr); \
   cairo_surface_t * out_s = cairo_surface_create_similar (window_surface, content, width, height); \
   cairo_destroy (wcr); \
   out_s; \
})
#endif


// GTK < 2.20
#if ! GTK_CHECK_VERSION (2, 20, 0)
#define gtk_widget_get_mapped(wid) ((GTK_WIDGET_FLAGS (wid) & GTK_MAPPED) != 0)
#define gtk_widget_get_realized(wid) ((GTK_WIDGET_FLAGS (wid) & GTK_REALIZED) != 0)
#define gtk_window_get_window_type(window) (GTK_WINDOW(window)->type)
#define gtk_widget_get_requisition(w,r) (*(r) = GTK_WIDGET(w)->requisition)
#define gtk_widget_set_mapped(w,yes) { \
   if (yes) GTK_WIDGET_SET_FLAGS(w,GTK_MAPPED); \
   else GTK_WIDGET_UNSET_FLAGS(w,GTK_MAPPED); \
}
#define gtk_widget_set_realized(w,yes) { \
   if (yes) GTK_WIDGET_SET_FLAGS(w,GTK_REALIZED); \
   else GTK_WIDGET_UNSET_FLAGS(w,GTK_REALIZED); \
}
#define gtk_range_get_slider_size_fixed(range) (GTK_RANGE(range)->slider_size_fixed)
#define gtk_range_get_min_slider_size(range) (GTK_RANGE(range)->min_slider_size)
#define gtk_entry_get_text_window(entry) (GTK_ENTRY(entry)->text_area)
#endif


// GTK < 2.18
#if ! GTK_CHECK_VERSION (2, 18, 0)
#define gtk_widget_get_state(wid) (GTK_WIDGET (wid)->state)
#define gtk_widget_is_toplevel(wid) ((GTK_WIDGET_FLAGS (wid) & GTK_TOPLEVEL) != 0)
#define gtk_widget_get_has_window(wid) !((GTK_WIDGET_FLAGS (wid) & GTK_NO_WINDOW) != 0)
#define gtk_widget_get_visible(wid) ((GTK_WIDGET_FLAGS (wid) & GTK_VISIBLE) != 0)
#define gtk_widget_is_drawable(wid)  (GTK_WIDGET_VISIBLE (wid) && GTK_WIDGET_MAPPED (wid))
#define gtk_widget_get_sensitive(wid) ((GTK_WIDGET_FLAGS (wid) & GTK_SENSITIVE) != 0)
#define gtk_widget_get_can_focus(wid) ((GTK_WIDGET_FLAGS (wid) & GTK_CAN_FOCUS) != 0)
#define gtk_widget_has_focus(wid) ((GTK_WIDGET_FLAGS (wid) & GTK_HAS_FOCUS) != 0)
#define gtk_widget_get_can_default(wid) ((GTK_WIDGET_FLAGS (wid) & GTK_CAN_DEFAULT) != 0)
#define gtk_widget_get_receives_default(wid) ((GTK_WIDGET_FLAGS (wid) & GTK_RECEIVES_DEFAULT) != 0)
#define gtk_widget_has_default(wid) ((GTK_WIDGET_FLAGS (wid) & GTK_HAS_DEFAULT) != 0)
#define gtk_widget_has_grab(wid) ((GTK_WIDGET_FLAGS (wid) & GTK_HAS_GRAB) != 0)
#define gtk_widget_get_app_paintable(wid) ((GTK_WIDGET_FLAGS (wid) & GTK_APP_PAINTABLE) != 0)
#define gtk_widget_get_double_buffered(wid) ((GTK_WIDGET_FLAGS (wid) & GTK_DOUBLE_BUFFERED) != 0)
#define gtk_widget_set_allocation(w,alloc) (GTK_WIDGET(w)->allocation = *(alloc))
#define gtk_widget_get_allocation(w,alloc) (*(alloc) = GTK_WIDGET(w)->allocation)
#define gtk_widget_set_can_default(w,yes) { \
   if (yes) GTK_WIDGET_SET_FLAGS(w,GTK_CAN_DEFAULT); \
   else GTK_WIDGET_UNSET_FLAGS(w,GTK_CAN_DEFAULT); \
}
#define gtk_widget_set_can_focus(w,yes) { \
   if (yes) GTK_WIDGET_SET_FLAGS(w,GTK_CAN_FOCUS); \
   else GTK_WIDGET_UNSET_FLAGS(w,GTK_CAN_FOCUS); \
}
#define gtk_widget_set_has_window(w,yes) { \
   if (yes) GTK_WIDGET_UNSET_FLAGS(w,GTK_NO_WINDOW); \
   else GTK_WIDGET_SET_FLAGS(w,GTK_NO_WINDOW); \
}
#define gtk_widget_set_visible(w,yes) { \
   if (yes) gtk_widget_show(w); \
   else gtk_widget_hide(w); \
}
#define gtk_range_get_flippable(range) (GTK_RANGE(range)->flippable)
#define gdk_window_is_destroyed(w) (GDK_WINDOW_DESTROYED (GDK_WINDOW(w)))
#endif


// GTK < 2.16
#if ! GTK_CHECK_VERSION (2, 16, 0)
#define gtk_status_icon_set_tooltip_text(icon,text) gtk_status_icon_set_tooltip(icon,text)
#define gtk_menu_item_get_label(i) (gtk_label_get_label (GTK_LABEL (GTK_BIN (i)->child)))
#define gtk_menu_item_set_label(i,label) gtk_label_set_label(GTK_LABEL(GTK_BIN(i)->child), (label) ? label : "")
#define gtk_menu_item_get_use_underline(i) (gtk_label_get_use_underline (GTK_LABEL (GTK_BIN (i)->child)))
#define gtk_status_icon_set_tooltip_text gtk_status_icon_set_tooltip
#endif


// GTK < 2.14
#if ! GTK_CHECK_VERSION (2, 14, 0)
#define gtk_dialog_get_action_area(dialog)    (GTK_DIALOG(dialog)->action_area)
#define gtk_dialog_get_content_area(dialog)   (GTK_DIALOG(dialog)->vbox)
#define gtk_widget_get_window(widget)         (GTK_WIDGET(widget)->window)
#define gtk_window_get_default_widget(window) (GTK_WINDOW(window)->default_widget)
#define gtk_menu_item_get_accel_path(i)       (GTK_MENU_ITEM(i)->accel_path)
#define gtk_menu_get_accel_path(menu)         (GTK_MENU(menu)->accel_path)
#define gtk_message_dialog_get_image(m)       (GTK_MESSAGE_DIALOG(m)->image)
#define gtk_entry_get_overwrite_mode(e)       (GTK_ENTRY(e)->overwrite_mode)
#endif


/* ================================================== */
/*                     GDK KEYS                       */
/* ================================================== */

#ifndef GDK_KEY_a
#	define GDK_KEY_Control_R GDK_Control_R
#	define GDK_KEY_Control_L GDK_Control_L
#	define GDK_KEY_Shift_R GDK_Shift_R
#	define GDK_KEY_Shift_L GDK_Shift_L
#	define GDK_KEY_Alt_R GDK_Alt_R
#	define GDK_KEY_Alt_L GDK_Alt_L
#	define GDK_KEY_Tab GDK_Tab
#	define GDK_KEY_space GDK_space
#	define GDK_KEY_Up GDK_Up
#	define GDK_KEY_Down GDK_Down
#	define GDK_KEY_Right GDK_Right
#	define GDK_KEY_Left GDK_Left
#	define GDK_KEY_Return GDK_Return
#	define GDK_KEY_exclam GDK_exclam
#	define GDK_KEY_BackSpace GDK_BackSpace
#	define GDK_KEY_Home GDK_Home
#	define GDK_KEY_End GDK_End
#	define GDK_KEY_Escape GDK_Escape
#	define GDK_KEY_a GDK_a
#	define GDK_KEY_A GDK_A
#	define GDK_KEY_b GDK_b
#	define GDK_KEY_B GDK_B
#	define GDK_KEY_c GDK_c
#	define GDK_KEY_C GDK_C
#	define GDK_KEY_d GDK_d
#	define GDK_KEY_D GDK_D
#	define GDK_KEY_e GDK_e
#	define GDK_KEY_E GDK_E
#	define GDK_KEY_f GDK_F
#	define GDK_KEY_g GDK_g
#	define GDK_KEY_G GDK_G
#	define GDK_KEY_h GDK_h
#	define GDK_KEY_H GDK_H
#	define GDK_KEY_i GDK_i
#	define GDK_KEY_I GDK_I
#	define GDK_KEY_j GDK_j
#	define GDK_KEY_J GDK_J
#	define GDK_KEY_k GDK_k
#	define GDK_KEY_K GDK_K
#	define GDK_KEY_l GDK_l
#	define GDK_KEY_L GDK_L
#	define GDK_KEY_m GDK_m
#	define GDK_KEY_M GDK_M
#	define GDK_KEY_n GDK_n
#	define GDK_KEY_N GDK_N
#	define GDK_KEY_o GDK_o
#	define GDK_KEY_O GDK_O
#	define GDK_KEY_p GDK_p
#	define GDK_KEY_P GDK_P
#	define GDK_KEY_q GDK_q
#	define GDK_KEY_Q GDK_Q
#	define GDK_KEY_r GDK_r
#	define GDK_KEY_R GDK_R
#	define GDK_KEY_s GDK_s
#	define GDK_KEY_S GDK_S
#	define GDK_KEY_t GDK_t
#	define GDK_KEY_T GDK_T
#	define GDK_KEY_u GDK_u
#	define GDK_KEY_U GDK_U
#	define GDK_KEY_v GDK_v
#	define GDK_KEY_V GDK_V
#	define GDK_KEY_w GDK_w
#	define GDK_KEY_W GDK_W
#	define GDK_KEY_x GDK_x
#	define GDK_KEY_X GDK_X
#	define GDK_KEY_y GDK_y
#	define GDK_KEY_Y GDK_Y
#	define GDK_KEY_z GDK_z
#	define GDK_KEY_Z GDK_Z
#	define GDK_KEY_exclam GDK_exclam
#	define GDK_KEY_F1 GDK_F1
#	define GDK_KEY_F2 GDK_F2
#	define GDK_KEY_F3 GDK_F3
#	define GDK_KEY_F4 GDK_F4
#	define GDK_KEY_F5 GDK_F5
#	define GDK_KEY_F6 GDK_F6
#	define GDK_KEY_F7 GDK_F7
#	define GDK_KEY_F8 GDK_F8
#	define GDK_KEY_F9 GDK_F9
#	define GDK_KEY_F10 GDK_F10
#	define GDK_KEY_F11 GDK_F11
#	define GDK_KEY_F12 GDK_F12
#	define GDK_KEY_KP_Home GDK_KP_Home
#	define GDK_KEY_KP_Page_Up GDK_KP_Page_Up
#	define GDK_KEY_KP_End GDK_KP_End
#	define GDK_KEY_KP_Page_Down GDK_KP_Page_Down
#	define GDK_KEY_KP_Space GDK_KP_Space
#	define GDK_KEY_KP_Enter GDK_KP_Enter
#	define GDK_KEY_KP_Left GDK_KP_Left
#	define GDK_KEY_KP_Right GDK_KP_Right
#	define GDK_KEY_KP_Up GDK_KP_Up
#	define GDK_KEY_KP_Down GDK_KP_Down
#endif


// CAIRO < 1.10
#if CAIRO_VERSION < CAIRO_VERSION_ENCODE(1, 10, 0)
#define cairo_region_t         GdkRegion
#define cairo_rectangle_int_t  GdkRectangle
#define cairo_region_create    gdk_region_new
#define cairo_region_copy      gdk_region_copy
#define cairo_region_destroy   gdk_region_destroy
#define cairo_region_create_rectangle gdk_region_rectangle
#define cairo_region_get_extents gdk_region_get_clipbox
#define cairo_region_is_empty  gdk_region_empty
#define cairo_region_equal     gdk_region_empty
#define cairo_region_contains_point gdk_region_point_in
#define cairo_region_contains_rectangle gdk_region_rect_in
#define cairo_region_translate gdk_region_rect_in
#define cairo_region_union_rectangle gdk_region_union_with_rect
#define cairo_region_intersect gdk_region_intersect
#define cairo_region_union     gdk_region_union
#define cairo_region_subtract  gdk_region_subtract
#define cairo_region_xor       gdk_region_xor
//#define cairo_region_num_rectangles / cairo_region_get_rectangle  gdk_region_get_rectangles
#endif


// PANGO
#ifndef PANGO_WEIGHT_MEDIUM
#define PANGO_WEIGHT_MEDIUM 500
#endif


#ifdef __cplusplus
}
#endif

#endif /* __GTKCOMPAT_H */
