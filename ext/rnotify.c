//
//            rnotify.c
//
//  Luca Russo <vargolo@gmail.com>
//  Copyright (LGPL) 2006 - 2012
//

#include <libnotify/notify.h>
#include "ruby.h"
#include "rbgobject.h"

#define MODULE_NAME "Notify"
#define CLASS_NAME "Notification"

typedef struct _ActionData ActionData;
struct _ActionData
{
  VALUE callback;
  VALUE action;
  VALUE user_data;
};

static void
_notification_action_cb(NotifyNotification *notify, const char *action,
                        ActionData *data)
{
  ActionData *tmp = data;

  if(tmp->user_data == Qnil)
    rb_funcall(tmp->callback, rb_intern("call"), 1, tmp->action);
  else
    rb_funcall(tmp->callback, rb_intern("call"), 2, tmp->action, tmp->user_data);
}

static void
_notification_action_free(ActionData *data)
{
  if(data != NULL)
  {
    g_free(data);
    data = NULL;
  }
}

static void
_wrap_alloc_free(VALUE self)
{
  NotifyNotification *n = NOTIFY_NOTIFICATION(RVAL2GOBJ(self));

  if(n != NULL)
  {
    g_object_unref(G_OBJECT(n));
    n = NULL;
  }
}

/*
 * call-seq:
 *      init(name)
 *
 * name = application name (required)
 *
 * Initialize libnotify. This must be called before any other functions.
 * Throw an expection If name is empty or nil
 *
 * Returns TRUE if the library initialized properly, FALSE otherwise
 */
static VALUE
_wrap_notify_init(VALUE UNUSED(self), VALUE app_name)
{
  gboolean init;
  char *name = NIL_P(app_name) ? NULL : StringValuePtr(app_name);

  if(name == NULL || *name == '\0')
    rb_raise(rb_eRuntimeError,
             "You MUST call this method with a valid string that will be used as Application name");
  else
    init = notify_init(name);

  if(init == TRUE)
    return Qtrue;

  return Qfalse;
}

/*
 * call-seq:
 *      uninit
 *
 * Deinitialize libnotify, you must to call this before quit the program
 */
static VALUE
_wrap_notify_uninit(VALUE UNUSED(self))
{
  notify_uninit();

  return Qnil;
}

/*
 * call-seq:
 *      init?
 *
 * Returns TRUE if libnotify is inizialized, FALSE otherwise
 */
static VALUE
_wrap_notify_is_initted(VALUE UNUSED(self))
{
  if(notify_is_initted())
    return Qtrue;

  return Qfalse;
}

/*
 * call-seq:
 *      name
 *
 * Returns the application name passed to Notify.init
 */
static VALUE
_wrap_notify_get_app_name(VALUE UNUSED(self))
{
  const gchar *name = notify_get_app_name();

  return rb_str_new2(name);
}

#ifdef HAVE_SET_APP_NAME
/*
 * call-seq:
 *      name= new_name
 *
 *  if new_name is a valid string, change the application name to new_name
 *  otherwise nothing will be changed
 * 
 *  ** ONLY WHEN COMPILED AGAINST LIBNOTIFY >= 0.7.2 **
 */
static VALUE
_wrap_notify_set_app_name(VALUE UNUSED(self), VALUE app_name)
{
  char *name = NIL_P(app_name) ? NULL : StringValuePtr(app_name);

  if(name != NULL || *name != '\0')
    notify_set_app_name(name);

  return Qnil;
}
#endif

/*
 * call-seq:
 *      server_caps
 *
 * Queries the server for its capabilities and returns them in an Array
 */
static VALUE
_wrap_notify_get_server_caps(VALUE UNUSED(self))
{
  GList *caps = NULL;
  VALUE rb_caps;

  caps = notify_get_server_caps();
  rb_caps = rb_ary_new();

  do
  {
      rb_ary_push(rb_caps, rb_str_new2(caps->data));
      caps = caps->next;
  } while(caps != NULL);

  g_list_foreach(caps, (GFunc)g_free, NULL);
  g_list_free(caps);

  return rb_caps;
}

/*
 * call-seq:
 *      server_info
 *
 * Queries the server for its information (name, vendor, server version,
 * notification version)
 *
 * Returns FALSE if there were errors, an Hash otherwise
 *
 * example:
 *
 *      h = Notify.server_info
 *      p h[:name]                #print the product name of the server
 *      p h[:vendor]              #print the vendor
 *      p h[:version]             #print the server version
 *      p h[:spec_version]        #print the specification version supported
 */
static VALUE
_wrap_notify_get_server_info(VALUE UNUSED(self))
{
  gchar *serv_name = NULL,
           *vendor = NULL,
           *serv_version = NULL,
           *spec_vers = NULL;
  VALUE rb_info;

  if(!notify_get_server_info(&serv_name, &vendor, &serv_version, &spec_vers))
    return Qfalse;
  else
  {
    rb_info = rb_hash_new();
    rb_hash_aset(rb_info, ID2SYM(rb_intern("name")), rb_str_new2(serv_name));
    rb_hash_aset(rb_info, ID2SYM(rb_intern("vendor")), rb_str_new2(vendor));
    rb_hash_aset(rb_info, ID2SYM(rb_intern("version")), rb_str_new2(serv_version));
    rb_hash_aset(rb_info, ID2SYM(rb_intern("spec_version")), rb_str_new2(spec_vers));

    g_free(serv_name);
    g_free(vendor);
    g_free(serv_version);
    g_free(spec_vers);
   }

  return rb_info;
}

/*
 * call-seq:
 *      new(summ, msg, icon)
 *
 * summ = The summary text (required)
 *
 * msg = The body text or nil
 *
 * icon = The icon or nil
 *
 * Creates and returns a new notification, throw an exception on error
 */
static VALUE
_wrap_notification_init(VALUE self, VALUE summ, VALUE msg, VALUE icon)
{
  char *nsumm = NIL_P(summ) ? NULL : StringValuePtr(summ);
  NotifyNotification *n = NULL;

  if(nsumm == NULL || *nsumm == '\0')
    rb_raise(rb_eArgError, "You need to supply a valid summary string");

  n = notify_notification_new(nsumm,
                              NIL_P(msg) ? NULL : StringValuePtr(msg),
                              NIL_P(icon) ? NULL : StringValuePtr(icon));

  if(n == NULL)
    rb_raise(rb_eRuntimeError, "Can not create a new notification");

  G_INITIALIZE(self, n);

#ifdef DEBUG
  rb_warn("init, ok");
#endif

  return self;
}

#ifdef HAVE_SET_APP_PNAME
/*
 * call-seq:
 *      name= new_name
 *
 *  if new_name is a valid string, sets the application name for the notification.
 *  otherwise nothing will be changed
 * 
 *  ** ONLY WHEN COMPILED AGAINST LIBNOTIFY >= 0.7.3 **
 */
static VALUE
_wrap_notification_set_app_pname(VALUE self, VALUE notification_name)
{
  NotifyNotification *n = NOTIFY_NOTIFICATION(RVAL2GOBJ(self));
  char *name = NIL_P(notification_name) ? NULL : StringValuePtr(notification_name);

  if(name != NULL || *name != '\0')
    notify_notification_set_app_name(n, name);

  return Qnil;
}
#endif

/*
 * call-seq:
 *      update(summ, msg, icon)
 *
 * summ = The new summary text (required)
 *
 * msg = The new body text or nil
 *
 * icon = The new icon or nil
 *
 * This won't send the update out and display it on the screen.
 * For that, you will need to call the Notification#show method.
 *
 * Returns TRUE if ok, FALSE otherwise
 */
static VALUE
_wrap_notification_update(VALUE self, VALUE summ, VALUE msg, VALUE icon)
{
  NotifyNotification *n = NOTIFY_NOTIFICATION(RVAL2GOBJ(self));
  char *nsumm = NIL_P(summ) ? NULL : StringValuePtr(summ);
  char *nmsg = NIL_P(msg) ? NULL : StringValuePtr(msg);
  char *nicon = NIL_P(icon) ? NULL : StringValuePtr(icon);

#ifdef DEBUG
  if(NOTIFY_IS_NOTIFICATION(n))
    rb_warn("update, ok");
  else
    rb_warn("update, no ok");
#endif

  if(nsumm == NULL || *nsumm == '\0')
    rb_raise(rb_eArgError, "You need to supply a valid summary string");

  if(notify_notification_update(n, nsumm, nmsg, nicon) == TRUE)
    return Qtrue;

  return Qfalse;
}

/*
 * call-seq:
 *      show
 *
 * Tells the notification server to display the notification on the screen.
 * if TRUE returns, show the notification otherwise returns FALSE
 */
static VALUE
_wrap_notification_show(VALUE self)
{
  NotifyNotification *n = NOTIFY_NOTIFICATION(RVAL2GOBJ(self));

#ifdef DEBUG
  if(NOTIFY_IS_NOTIFICATION(n))
    rb_warn("show, ok");
  else
    rb_warn("show, no ok");
#endif

  if(notify_notification_show(n, NULL) == TRUE)
    return Qtrue;

  return Qfalse;
}

/*
 * call-seq:
 *      timeout=(milliseconds)
 *
 * Sets the timeout in milliseconds.
 */
static VALUE
_wrap_notification_set_timeout(VALUE self, VALUE ml)
{
  NotifyNotification *n = NOTIFY_NOTIFICATION(RVAL2GOBJ(self));

#ifdef DEBUG
  if(NOTIFY_IS_NOTIFICATION(n))
    rb_warn("timeout, ok");
  else
    rb_warn("timeout, no ok");
#endif

  notify_notification_set_timeout(n, FIX2INT(ml));

  return Qnil;
}

/*
 * call-seq:
 *      category=(category_name)
 *
 * category_name = The category name
 *
 * Sets the category
 */
static VALUE
_wrap_notification_set_category(VALUE self, VALUE cat_name)
{
  char *cname = NIL_P(cat_name) ? NULL : StringValuePtr(cat_name);
  NotifyNotification *n = NOTIFY_NOTIFICATION(RVAL2GOBJ(self));

#ifdef DEBUG
  if(NOTIFY_IS_NOTIFICATION(n))
    rb_warn("set_category, ok");
  else
    rb_warn("set_category, not ok");
#endif

  notify_notification_set_category(n, cname);

  return Qnil;
}

/*
 * call-seq:
 *      urgency=(urgency_level)
 *
 * urgency_level = The urgency level
 *
 * Sets the urgency level
 */
static VALUE
_wrap_notification_set_urgency(VALUE self, VALUE urgency)
{
  NotifyNotification *n = NOTIFY_NOTIFICATION(RVAL2GOBJ(self));

#ifdef DEBUG
  if(NOTIFY_IS_NOTIFICATION(n))
    rb_warn("set_urgency, ok");
  else
    rb_warn("set_urgency, no ok");
#endif

  switch(FIX2INT(urgency))
  {
  case NOTIFY_URGENCY_LOW:
    notify_notification_set_urgency(n, NOTIFY_URGENCY_LOW);
    break;
  case NOTIFY_URGENCY_NORMAL:
    notify_notification_set_urgency(n, NOTIFY_URGENCY_NORMAL);
    break;
  case NOTIFY_URGENCY_CRITICAL:
    notify_notification_set_urgency(n, NOTIFY_URGENCY_CRITICAL);
    break;
  default:
    notify_notification_set_urgency(n, NOTIFY_URGENCY_NORMAL);
  }

  return Qnil;
}

/*
 * call-seq:
 *      pixbuf_icon=(icon)
 *
 * icon = The icon
 *
 * Sets the icon from a Gdk::Pixbuf
 */
static VALUE
_wrap_notification_set_pixbuf_icon(VALUE self, VALUE icon)
{
  GdkPixbuf *pix = GDK_PIXBUF(RVAL2GOBJ(icon));
  NotifyNotification *n = NOTIFY_NOTIFICATION(RVAL2GOBJ(self));

#ifdef DEBUG
  if(NOTIFY_IS_NOTIFICATION(n) && GDK_IS_PIXBUF(pix))
    rb_warn("pixbuf_icon, ok");
  else
    rb_warn("pixbuf_icon, no ok");
#endif

  notify_notification_set_image_from_pixbuf(n, pix);

  return Qnil;
}

/*
* call-seq:
* hint32(key, value)
*
* key = The hint
*
* value = The hint's value
*
* Sets a hint with a 32-bit integer value
*/
static VALUE
_wrap_notification_set_hint32(VALUE self, VALUE key, VALUE value)
{
  char *vkey = NIL_P(key) ? NULL : StringValuePtr(key);
  NotifyNotification *n = NOTIFY_NOTIFICATION(RVAL2GOBJ(self));

#ifdef DEBUG
  if(NOTIFY_IS_NOTIFICATION(n))
    rb_warn("set_hint32, ok");
  else
    rb_warn("set_hint32, no ok");
#endif

  notify_notification_set_hint_int32(n, vkey, FIX2INT(value));

  return Qnil;
}

/*
* call-seq:
* hint_double(key, value)
*
* key = The hint
*
* value = The hint's value
*
* Sets a hint with a double value
*/
static VALUE
_wrap_notification_set_hint_double(VALUE self, VALUE key, VALUE value)
{
  char *vkey = NIL_P(key) ? NULL : StringValuePtr(key);
  NotifyNotification *n = NOTIFY_NOTIFICATION(RVAL2GOBJ(self));

#ifdef DEBUG
  if(NOTIFY_IS_NOTIFICATION(n))
    rb_warn("set_hint_double, ok");
  else
    rb_warn("set_hint_double, no ok");
#endif

  notify_notification_set_hint_double(n, vkey, NUM2DBL(value));

  return Qnil;
}

/*
* call-seq:
* hint_string(key, value)
*
* key = The hint
*
* value = The hint's value
*
* Sets a hint with a string value
*/
static VALUE
_wrap_notification_set_hint_string(VALUE self, VALUE key, VALUE value)
{
  char *vkey = NIL_P(key) ? NULL : StringValuePtr(key);
  char *vvalue = NIL_P(value) ? NULL : StringValuePtr(value);
  NotifyNotification *n = NOTIFY_NOTIFICATION(RVAL2GOBJ(self));

#ifdef DEBUG
  if(NOTIFY_IS_NOTIFICATION(n))
    rb_warn("set_hint_string, ok");
  else
    rb_warn("set_hint_string, no ok");
#endif

  notify_notification_set_hint_string(n, vkey, vvalue);

  return Qnil;
}

/*
* call-seq:
* hint_byte(key, value)
*
* key = The hint
*
* value = The hint's value
*
* Sets a hint with a byte value
*/
static VALUE
_wrap_notification_set_hint_byte(VALUE self, VALUE key, VALUE value)
{
  char *vkey = NIL_P(key) ? NULL : StringValuePtr(key);
  NotifyNotification *n = NOTIFY_NOTIFICATION(RVAL2GOBJ(self));

#ifdef DEBUG
  if(NOTIFY_IS_NOTIFICATION(n))
    rb_warn("set_hint_byte, ok");
  else
    rb_warn("set_hint_byte, no ok");
#endif

  notify_notification_set_hint_byte(n, vkey, FIX2INT(value));

  return Qnil;
}

/*
 * call-seq:
 *      clear_hints
 *
 * Clears all hints from the notification. Remember to use this method before call the Notification#close method.
 */
static VALUE
_wrap_notification_clear_hints(VALUE self)
{
  NotifyNotification *n = NOTIFY_NOTIFICATION(RVAL2GOBJ(self));

#ifdef DEBUG
  if(NOTIFY_IS_NOTIFICATION(n))
    rb_warn("clear_hints, ok");
  else
    rb_warn("clear_hints, no ok");
#endif

  notify_notification_clear_hints(n);

  return Qnil;
}

/*
 * call-seq:
 *      add_action(action, label, user_data) { |action, user_data| ... }
 *
 * action = The action id
 *
 * label = The action label
 *
 * user_data = Custom data to pass into the block (optional)
 *
 * Adds an action. When the action is invoked, the specified block will be called
 *
 * Examples:
 *
 * myinstance.add_action( "MyAction", "MyLabel" ) do |action|
 *      # something to do
 * end
 *
 * Or
 *
 * myinstance.add_action( "MyAction", "MyLabel", MyData ) do |action, mydata|
 *      # something to do
 * end
 */
static VALUE
_wrap_notification_add_action(int argc, VALUE *argv, VALUE self)
{
  NotifyNotification *n = NOTIFY_NOTIFICATION(RVAL2GOBJ(self));
  VALUE action, label, data, body;
  ActionData *actionData = NULL;

#ifdef DEBUG
  if(NOTIFY_IS_NOTIFICATION(n))
    rb_warn("add_action, ok");
  else
    rb_warn("add_action, no ok");
#endif

  if(!rb_block_given_p())
    rb_raise(rb_eRuntimeError, "This method requires a block");

  rb_scan_args(argc, argv, "21&", &action, &label, &data, &body);

  actionData = g_new0(ActionData, 1);
  actionData->callback = body;
  actionData->action = action;
  actionData->user_data = data;

  notify_notification_add_action(n,
                                 NIL_P(action) ? NULL : StringValuePtr(action),
                                 NIL_P(label) ? NULL : StringValuePtr(label),
                                 NOTIFY_ACTION_CALLBACK(_notification_action_cb),
                                 actionData, (GFreeFunc)_notification_action_free);

  return Qnil;
}

/*
 * call-seq:
 *      clear_actions
 *
 * Clears all actions from the notification. Remember to use this method before call the Notification#close method.
 */
static VALUE
_wrap_notification_clear_actions(VALUE self)
{
  NotifyNotification *n = NOTIFY_NOTIFICATION(RVAL2GOBJ(self));

#ifdef DEBUG
  if(NOTIFY_IS_NOTIFICATION(n))
    rb_warn("clear_actions, ok");
  else
    rb_warn("clear_actions, no ok");
#endif

  notify_notification_clear_actions(n);

  return Qnil;
}

/*
 * call-seq:
 *      close
 *
 * Tells the notification server to hide the notification on the screen.
 */
static VALUE
_wrap_notification_close(VALUE self)
{
  NotifyNotification *n = NOTIFY_NOTIFICATION(RVAL2GOBJ(self));

#ifdef DEBUG
  if(NOTIFY_IS_NOTIFICATION(n))
    rb_warn("close, ok");
  else
    rb_warn("close, no ok");
#endif

  if(notify_notification_close(n, NULL))
    return Qtrue;

  return Qfalse;
}

/*
 * call-seq:
 *       closed_reason
 *
 * Returns the reason code why the notification was closed
 */
static VALUE
_wrap_notification_get_closed_reason(VALUE self)
{
  NotifyNotification *n = NOTIFY_NOTIFICATION(RVAL2GOBJ(self));
  int reason = -1;

#ifdef DEBUG
  if(NOTIFY_IS_NOTIFICATION(n))
    rb_warn("closed_reason, ok");
  else
    rb_warn("closed_reason, no ok");
#endif

  reason = notify_notification_get_closed_reason(n);

  return INT2FIX(reason);
}

/*
 * libnotify ruby interface
 * [ http://ftp.gnome.org/pub/GNOME/sources/libnotify ]
 * [ http://www.galago-project.org ]
 */
void
Init_rnotify()
{
  g_type_init();
  VALUE mNotify = rb_define_module(MODULE_NAME);
  VALUE cNotification = G_DEF_CLASS2(NOTIFY_TYPE_NOTIFICATION, CLASS_NAME, mNotify,
                                     0, _wrap_alloc_free);

  rb_define_const(cNotification, "URGENCY_LOW", INT2FIX(NOTIFY_URGENCY_LOW));
  rb_define_const(cNotification, "URGENCY_NORMAL", INT2FIX(NOTIFY_URGENCY_NORMAL));
  rb_define_const(cNotification, "URGENCY_CRITICAL", INT2FIX(NOTIFY_URGENCY_CRITICAL));
  rb_define_const(cNotification, "EXPIRES_DEFAULT", INT2FIX(NOTIFY_EXPIRES_DEFAULT));
  rb_define_const(cNotification, "EXPIRES_NEVER", INT2FIX(NOTIFY_EXPIRES_NEVER));

  rb_define_module_function(mNotify, "init", _wrap_notify_init, 1);
  rb_define_module_function(mNotify, "uninit", _wrap_notify_uninit, 0);
  rb_define_module_function(mNotify, "init?", _wrap_notify_is_initted, 0);
  rb_define_module_function(mNotify, "name", _wrap_notify_get_app_name, 0);
#ifdef HAVE_SET_APP_NAME
  rb_define_module_function(mNotify, "name=", _wrap_notify_set_app_name, 1);
#endif
  rb_define_module_function(mNotify, "server_caps", _wrap_notify_get_server_caps, 0);
  rb_define_module_function(mNotify, "server_info", _wrap_notify_get_server_info, 0);

  rb_define_method(cNotification, "initialize", _wrap_notification_init, 3);
#ifdef HAVE_SET_APP_PNAME
  rb_define_method(cNotification, "name=", _wrap_notification_set_app_pname, 1);
#endif
  rb_define_method(cNotification, "update", _wrap_notification_update, 3);
  rb_define_method(cNotification, "show", _wrap_notification_show, 0);
  rb_define_method(cNotification, "timeout=", _wrap_notification_set_timeout, 1);
  rb_define_method(cNotification, "category=", _wrap_notification_set_category, 1);
  rb_define_method(cNotification, "urgency=", _wrap_notification_set_urgency, 1);
  rb_define_method(cNotification, "pixbuf_icon=", _wrap_notification_set_pixbuf_icon, 1);
  rb_define_method(cNotification, "hint32", _wrap_notification_set_hint32, 2);
  rb_define_method(cNotification, "hint_double", _wrap_notification_set_hint_double, 2);
  rb_define_method(cNotification, "hint_string", _wrap_notification_set_hint_string, 2);
  rb_define_method(cNotification, "hint_byte", _wrap_notification_set_hint_byte, 2);
  rb_define_method(cNotification, "add_action", _wrap_notification_add_action, -1);
  rb_define_method(cNotification, "clear_actions", _wrap_notification_clear_actions, 0);
  rb_define_method(cNotification, "clear_hints", _wrap_notification_clear_hints, 0);
  rb_define_method(cNotification, "closed_reason", _wrap_notification_get_closed_reason, 0);
  rb_define_method(cNotification, "close", _wrap_notification_close, 0);
}
