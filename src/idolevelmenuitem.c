/*
 * Copyright 2023 Robert Tari
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Robert Tari <robert@tari.in>
 */

#include "idolevelmenuitem.h"
#include "idoactionhelper.h"

enum
{
    PROP_0,
    PROP_ICON,
    PROP_TEXT,
    PROP_LEVEL,
    PROP_LAST
};

static GParamSpec *lProperties[PROP_LAST];

typedef struct
{
    GIcon *pIcon;
    gchar *sText;
    GtkWidget *pLabel;
    GtkWidget *pImage;
    GtkWidget *pLevelBar;
    IdoActionHelper *pHelper;

} IdoLevelMenuItemPrivate;

static GParamSpec *lProperties[PROP_LAST];

G_DEFINE_TYPE_WITH_PRIVATE (IdoLevelMenuItem, ido_level_menu_item, GTK_TYPE_MENU_ITEM)

static void onGetProperty (GObject *pObject, guint nProperty, GValue *pValue, GParamSpec *pParamSpec)
{
    IdoLevelMenuItem *self = IDO_LEVEL_MENU_ITEM (pObject);
    IdoLevelMenuItemPrivate *pPrivate = ido_level_menu_item_get_instance_private (self);

    switch (nProperty)
    {
        case PROP_ICON:
        {
            g_value_set_object (pValue, pPrivate->pIcon);

            break;
        }
        case PROP_TEXT:
        {
            g_value_set_string (pValue, pPrivate->sText);

            break;
        }
        case PROP_LEVEL:
        {
            guint16 nLevel = (guint16) gtk_level_bar_get_value (GTK_LEVEL_BAR (pPrivate->pLevelBar));
            g_value_set_uint (pValue, (guint16) nLevel);

            break;
        }
        default:
        {
            G_OBJECT_WARN_INVALID_PROPERTY_ID (pObject, nProperty, pParamSpec);

            break;
        }
    }
}

static void onSetProperty (GObject *pObject, guint nProperty, const GValue *pValue, GParamSpec *pParamSpec)
{
    IdoLevelMenuItem *self = IDO_LEVEL_MENU_ITEM(pObject);

    switch (nProperty)
    {
        case PROP_ICON:
        {
            idoLevelMenuItemSetIcon (self, g_value_get_object (pValue));

            break;
        }
        case PROP_TEXT:
        {
            idoLevelMenuItemSetText (self, g_value_get_string (pValue));

            break;
        }
        case PROP_LEVEL:
        {
            idoLevelMenuItemSetLevel (self, g_value_get_uint (pValue));

            break;
        }
        default:
        {
            G_OBJECT_WARN_INVALID_PROPERTY_ID (pObject, nProperty, pParamSpec);

            break;
        }
    }
}

static void onDispose (GObject *pObject)
{
    IdoLevelMenuItem *self = IDO_LEVEL_MENU_ITEM (pObject);
    IdoLevelMenuItemPrivate *pPrivate = ido_level_menu_item_get_instance_private (self);

    g_clear_object (&pPrivate->pIcon);
    G_OBJECT_CLASS (ido_level_menu_item_parent_class)->dispose(pObject);
}

static void onFinalize(GObject *pObject)
{
    IdoLevelMenuItem *self = IDO_LEVEL_MENU_ITEM (pObject);
    IdoLevelMenuItemPrivate *pPrivate = ido_level_menu_item_get_instance_private (self);

    g_free (pPrivate->sText);
    G_OBJECT_CLASS (ido_level_menu_item_parent_class)->finalize (pObject);
}

static void idoLevelMenuItemStyleUpdateImage (IdoLevelMenuItem *self)
{
    IdoLevelMenuItemPrivate *pPrivate = ido_level_menu_item_get_instance_private (self);

    gtk_image_clear (GTK_IMAGE (pPrivate->pImage));

    if (pPrivate->pIcon == NULL)
    {
        gtk_widget_set_visible (pPrivate->pImage, FALSE);
    }
    else
    {
        gtk_image_set_from_gicon (GTK_IMAGE (pPrivate->pImage), pPrivate->pIcon, GTK_ICON_SIZE_MENU);
        gtk_widget_set_visible (pPrivate->pImage, TRUE);
    }
}

static void onStyleUpdated (GtkWidget *pWidget)
{
    GTK_WIDGET_CLASS (ido_level_menu_item_parent_class)->style_updated (pWidget);
    idoLevelMenuItemStyleUpdateImage (IDO_LEVEL_MENU_ITEM (pWidget));
    gtk_widget_queue_draw (pWidget);
}

static void onActivate (GtkMenuItem *item, gpointer pData)
{
    IdoActionHelper *pHelper = pData;
    ido_action_helper_activate (pHelper);
}

static void ido_level_menu_item_class_init (IdoLevelMenuItemClass *klass)
{
    GObjectClass *pObject = G_OBJECT_CLASS (klass);
    GtkWidgetClass *pWidget = GTK_WIDGET_CLASS (klass);
    pObject->get_property = onGetProperty;
    pObject->set_property = onSetProperty;
    pObject->dispose = onDispose;
    pObject->finalize = onFinalize;
    pWidget->style_updated = onStyleUpdated;

    GParamFlags nParamFlags = G_PARAM_CONSTRUCT | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS;
    lProperties[PROP_ICON] = g_param_spec_object ("icon", "Icon", "The menuitem's GIcon", G_TYPE_OBJECT, nParamFlags);
    lProperties[PROP_TEXT] = g_param_spec_string ("text", "Text", "The menuitem's text", "", nParamFlags);
    lProperties[PROP_LEVEL] = g_param_spec_uint ("level", "Level", "The menuitem's level bar value", 0, 100, 0, nParamFlags);

    g_object_class_install_properties(pObject, PROP_LAST, lProperties);
}

static void ido_level_menu_item_init (IdoLevelMenuItem *self)
{
    IdoLevelMenuItemPrivate *pPrivate = ido_level_menu_item_get_instance_private (self);
    pPrivate->pHelper = NULL;
    pPrivate->pImage = gtk_image_new();
    pPrivate->pLabel = gtk_label_new("");
    pPrivate->pLevelBar = gtk_level_bar_new_for_interval (0.0, 100.0);
    gtk_level_bar_add_offset_value (GTK_LEVEL_BAR (pPrivate->pLevelBar), GTK_LEVEL_BAR_OFFSET_LOW, 20.0);
    gtk_level_bar_add_offset_value (GTK_LEVEL_BAR (pPrivate->pLevelBar), GTK_LEVEL_BAR_OFFSET_HIGH, 90.0);
    gtk_level_bar_add_offset_value (GTK_LEVEL_BAR (pPrivate->pLevelBar), GTK_LEVEL_BAR_OFFSET_FULL, 100.0);

    GtkWidget *pGrid = gtk_grid_new ();
    gtk_grid_attach (GTK_GRID (pGrid), pPrivate->pImage, 0, 0, 1, 2);
    gtk_grid_attach (GTK_GRID (pGrid), pPrivate->pLabel, 1, 0, 1, 1);
    gtk_grid_attach (GTK_GRID (pGrid), pPrivate->pLevelBar, 1, 1, 1, 1);

    g_object_set (pPrivate->pImage, "halign", GTK_ALIGN_START, "hexpand", FALSE, "valign", GTK_ALIGN_START, "margin-right", 6, NULL);
    g_object_set (pPrivate->pLabel, "halign", GTK_ALIGN_START, "hexpand", TRUE, "valign", GTK_ALIGN_CENTER, NULL);

    gtk_widget_show_all (pGrid);
    gtk_container_add (GTK_CONTAINER(self), pGrid);
}

GtkWidget* ido_level_menu_item_new ()
{
    return GTK_WIDGET (g_object_new (IDO_TYPE_LEVEL_MENU_ITEM, NULL));
}

void idoLevelMenuItemSetIcon (IdoLevelMenuItem *self, GIcon *pIcon)
{
    IdoLevelMenuItemPrivate *pPrivate = ido_level_menu_item_get_instance_private (self);

    if (pPrivate->pIcon != pIcon)
    {
        if (pPrivate->pIcon)
        {
            g_object_unref (pPrivate->pIcon);
        }

        pPrivate->pIcon = pIcon ? g_object_ref (pIcon) : NULL;
        idoLevelMenuItemStyleUpdateImage (self);
    }
}

void idoLevelMenuItemSetText (IdoLevelMenuItem *self, const char *sText)
{
    IdoLevelMenuItemPrivate *pPrivate = ido_level_menu_item_get_instance_private (self);

    if (g_strcmp0(pPrivate->sText, sText))
    {
        g_free (pPrivate->sText);
        pPrivate->sText = g_strdup (sText);

        g_object_set (G_OBJECT (pPrivate->pLabel), "label", pPrivate->sText, "visible", (gboolean)(pPrivate->sText && *pPrivate->sText), NULL);
    }
}

void idoLevelMenuItemSetLevel (IdoLevelMenuItem *self, guint16 nLevel)
{
    IdoLevelMenuItemPrivate *pPrivate = ido_level_menu_item_get_instance_private (self);

    gtk_level_bar_set_value (GTK_LEVEL_BAR (pPrivate->pLevelBar), (gdouble)nLevel);
}

GtkMenuItem* ido_level_menu_item_new_from_model (GMenuItem *pMenuItem, GActionGroup *pActionGroup)
{
    GtkWidget *pItem = ido_level_menu_item_new ();
    IdoLevelMenuItemPrivate *pPrivate = ido_level_menu_item_get_instance_private (IDO_LEVEL_MENU_ITEM (pItem));

    gchar *sLabel;

    if (g_menu_item_get_attribute (pMenuItem, "label", "s", &sLabel))
    {
        idoLevelMenuItemSetText (IDO_LEVEL_MENU_ITEM (pItem), sLabel);
        g_free (sLabel);
    }

    GVariant *sIcon = g_menu_item_get_attribute_value (pMenuItem, "icon", NULL);

    if (sIcon)
    {
        GIcon *pIcon = g_icon_deserialize (sIcon);
        idoLevelMenuItemSetIcon (IDO_LEVEL_MENU_ITEM (pItem), pIcon);
        g_object_unref (pIcon);
        g_variant_unref (sIcon);
    }

    guint16 nProgress = 0;

    if (g_menu_item_get_attribute (pMenuItem, "x-ayatana-level", "q", &nProgress))
    {
        idoLevelMenuItemSetLevel (IDO_LEVEL_MENU_ITEM (pItem), nProgress);
    }

    gchar *sAction;

    if (g_menu_item_get_attribute (pMenuItem, "action", "s", &sAction))
    {
        GVariant *sTarget = g_menu_item_get_attribute_value (pMenuItem, "target", NULL);
        pPrivate->pHelper = ido_action_helper_new (pItem, pActionGroup, sAction, sTarget);
        g_signal_connect_object (pItem, "activate", G_CALLBACK (onActivate), pPrivate->pHelper, 0);
        g_signal_connect_swapped (pItem, "destroy", G_CALLBACK (g_object_unref), pPrivate->pHelper);

        if (sTarget)
        {
            g_variant_unref (sTarget);
        }

        g_free (sAction);
    }

    return GTK_MENU_ITEM (pItem);
}
