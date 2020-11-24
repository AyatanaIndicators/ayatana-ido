#ifndef __IDO_REMOVABLE_MENU_ITEM_H__
#define __IDO_REMOVABLE_MENU_ITEM_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define IDO_TYPE_REMOVABLE_MENU_ITEM (ido_removable_menu_item_get_type())
#define IDO_REMOVABLE_MENU_ITEM(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), IDO_TYPE_REMOVABLE_MENU_ITEM, IdoRemovableMenuItem))
#define IDO_IS_REMOVABLE_MENU_ITEM(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), IDO_TYPE_REMOVABLE_MENU_ITEM))

typedef struct _IdoRemovableMenuItem IdoRemovableMenuItem;
typedef struct _IdoRemovableMenuItemClass IdoRemovableMenuItemClass;

struct _IdoRemovableMenuItemClass
{
    GtkMenuItemClass parent_class;
};


struct _IdoRemovableMenuItem
{
    GtkMenuItem parent;
};


GType ido_removable_menu_item_get_type()G_GNUC_CONST;
GtkWidget * ido_removable_menu_item_new();
void idoRemovableMenuItemSetIcon(IdoRemovableMenuItem *self, GIcon *pIcon);
void idoRemovableMenuItemSetIconFromFile(IdoRemovableMenuItem *self, const char *sFilename);
void idoRemovableMenuItemSetText(IdoRemovableMenuItem *self, const char *sText);
void idoRemovableMenuItemUseMarkup(IdoRemovableMenuItem *self, gboolean bUse);
GtkMenuItem* ido_removable_menu_item_new_from_model(GMenuItem *pMenuItem, GActionGroup *pActionGroup);

G_END_DECLS

#endif
