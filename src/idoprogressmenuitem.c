/*
 * Copyright 2023 Robert Tari
 *
 * Authors:
 *   Robert Tari <robert@tari.in>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "idoprogressmenuitem.h"
#include "idobasicmenuitem.h"
#include "idoactionhelper.h"

static void onActivate (GtkMenuItem *item, gpointer pData)
{
    IdoActionHelper *pHelper = pData;
    ido_action_helper_activate (pHelper);
}

/**
 * ido_progress_menu_item_new_from_model:
 * @pMenuItem: the corresponding menuitem
 * @pActionGroup: action group to tell when this GtkMenuItem is activated
 *
 * Creates a new progress menuitem with properties initialized from
 * the menuitem's attributes.
 *
 * If the menuitem's 'action' attribute is set, trigger that action
 * in @pActionGroup when this IdoProgressMenuItem is activated.
 */
GtkMenuItem *ido_progress_menu_item_new_from_model (GMenuItem *pMenuItem, GActionGroup *pActionGroup)
{
    IdoBasicMenuItem *pBasicMenuItem = NULL;
    gchar *sLabel = NULL;

    if (g_menu_item_get_attribute (pMenuItem, "label", "s", &sLabel))
    {
        pBasicMenuItem = IDO_BASIC_MENU_ITEM (g_object_new (IDO_TYPE_BASIC_MENU_ITEM, "text", sLabel, NULL));
        g_free (sLabel);

        GVariant *pIconVariant = g_menu_item_get_attribute_value (pMenuItem, "icon", NULL);

        if (pIconVariant)
        {
            GIcon *pIcon = g_icon_deserialize (pIconVariant);
            ido_basic_menu_item_set_icon (pBasicMenuItem, pIcon);
            g_object_unref (pIcon);
            g_variant_unref (pIconVariant);
        }

        guint16 nProgress = 0;

        if (g_menu_item_get_attribute (pMenuItem, "x-ayatana-progress", "q", &nProgress))
        {
            gchar *sProgress = g_strdup_printf ("%"G_GUINT16_FORMAT"%%", nProgress);
            ido_basic_menu_item_set_secondary_text (pBasicMenuItem, sProgress);
            g_free (sProgress);
        }

        gchar *sAction = NULL;

        if (g_menu_item_get_attribute (pMenuItem, "action", "s", &sAction))
        {
            GVariant *pTarget = g_menu_item_get_attribute_value (pMenuItem, "target", NULL);
            IdoActionHelper *pHelper = ido_action_helper_new (GTK_WIDGET (pBasicMenuItem), pActionGroup, sAction, pTarget);
            g_signal_connect_object (pBasicMenuItem, "activate", G_CALLBACK (onActivate), pHelper, 0);
            g_signal_connect_swapped (pBasicMenuItem, "destroy", G_CALLBACK (g_object_unref), pHelper);

            if (pTarget)
            {
                g_variant_unref (pTarget);
            }

            g_free (sAction);
        }
    }

    return GTK_MENU_ITEM (pBasicMenuItem);
}

