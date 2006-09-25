/* This file is part of GEGL
 *
 * GEGL is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * GEGL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEGL; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Copyright 2003 Calvin Williamson
 */

#include "config.h"

#include <glib-object.h>
#include <string.h>

#include "gegl-types.h"

#include "gegl-eval-visitor.h"
#include "gegl-operation.h"
#include "gegl-node.h"
#include "gegl-pad.h"
#include "gegl-visitable.h"
#include "gegl-instrument.h"


static void gegl_eval_visitor_class_init (GeglEvalVisitorClass *klass);
static void visit_pad                    (GeglVisitor          *self,
                                          GeglPad              *pad);


G_DEFINE_TYPE(GeglEvalVisitor, gegl_eval_visitor, GEGL_TYPE_VISITOR)


static void
gegl_eval_visitor_class_init (GeglEvalVisitorClass *klass)
{
  GeglVisitorClass *visitor_class = GEGL_VISITOR_CLASS (klass);

  visitor_class->visit_pad = visit_pad;
}

static void
gegl_eval_visitor_init (GeglEvalVisitor *self)
{
}

extern long babl_total_usecs;

static void
visit_pad (GeglVisitor *self,
           GeglPad     *pad)
{
  GeglNode      *node      = gegl_pad_get_node (pad);
  GeglOperation *operation = node->operation;

  GEGL_VISITOR_CLASS (gegl_eval_visitor_parent_class)->visit_pad (self, pad);

  if (gegl_pad_is_output (pad))
    {
      glong time = gegl_ticks ();
      glong babl_time = babl_total_usecs;
      gegl_operation_process (operation, gegl_pad_get_name (pad));
      babl_time = babl_total_usecs - babl_time;
      time = gegl_ticks ()-time;

      gegl_instrument ("process", gegl_node_get_op_type_name (node), time);
      gegl_instrument (gegl_node_get_op_type_name (node), "babl", babl_time);
    }
  else if (gegl_pad_is_input (pad))
    {
      GeglPad *source_pad = gegl_pad_get_real_connected_to (pad);

      if (source_pad)
        {
          GValue      value     = { 0 };
          GParamSpec *prop_spec = gegl_pad_get_param_spec (pad);
          GeglNode *source_node = gegl_pad_get_node (source_pad);
          GeglOperation *source = source_node->operation;

          g_value_init (&value, G_PARAM_SPEC_VALUE_TYPE (prop_spec));

          g_object_get_property (G_OBJECT(source),
                                 gegl_pad_get_name (source_pad),
                                 &value);

          if (!g_value_get_object (&value) &&
              !g_object_get_data (G_OBJECT (source_node), "graph"))
             g_warning ("eval-visitor encountered a NULL buffer passed from: %s.%s-[%p]", 
             gegl_node_get_debug_name (source_node),
             gegl_pad_get_name (source_pad),
             g_value_get_object (&value));

          g_object_set_property (G_OBJECT (operation),
                                 gegl_pad_get_name (pad),
                                 &value);

          /* reference counting for this source dropped to zero, freeing up */
          if (--gegl_pad_get_node (source_pad)->refs==0 &&
              g_value_get_object (&value))
            {
              g_object_unref (g_value_get_object (&value));
            }

          g_value_unset (&value);
        }
    }
}
