/*
 * This file is a part of the C port of the Poly2Tri library
 * Porting to C done by (c) Barak Itkin <lightningismyname@gmail.com>
 * http://code.google.com/p/poly2tri-c/
 *
 * Poly2Tri Copyright (c) 2009-2010, Poly2Tri Contributors
 * http://code.google.com/p/poly2tri/
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * * Neither the name of Poly2Tri nor the names of its contributors may be
 *   used to endorse or promote products derived from this software without specific
 *   prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <math.h>
#include "utils.h"

/**
 * Forumla to calculate signed area<br>
 * Positive if CCW<br>
 * Negative if CW<br>
 * 0 if collinear<br>
 * <pre>
 * A[P1,P2,P3]  =  (x1*y2 - y1*x2) + (x2*y3 - y2*x3) + (x3*y1 - y3*x1)
 *              =  (x1-x3)*(y2-y3) - (y1-y3)*(x2-x3)
 * </pre>
 */
P2tOrientation
p2t_orient2d (P2tPoint* pa, P2tPoint* pb, P2tPoint* pc)
{
  double detleft = (pa->x - pc->x) * (pb->y - pc->y);
  double detright = (pa->y - pc->y) * (pb->x - pc->x);
  double val = detleft - detright;
  if (val > -EPSILON && val < EPSILON)
    {
      return COLLINEAR;
    }
  else if (val > 0)
    {
      return CCW;
    }
  return CW;
}

gboolean
p2t_utils_in_scan_area (P2tPoint* pa, P2tPoint* pb, P2tPoint* pc, P2tPoint* pd)
{
#if FALSE
  double pdx = pd->x;
  double pdy = pd->y;
  double adx = pa->x - pdx;
  double ady = pa->y - pdy;
  double bdx = pb->x - pdx;
  double bdy = pb->y - pdy;

  double adxbdy = adx * bdy;
  double bdxady = bdx * ady;
  double oabd = adxbdy - bdxady;

  double cdx, cdy;
  double cdxady, adxcdy, ocad;

  if (oabd <= EPSILON)
    {
      return FALSE;
    }

  cdx = pc->x - pdx;
  cdy = pc->y - pdy;

  cdxady = cdx * ady;
  adxcdy = adx * cdy;
  ocad = cdxady - adxcdy;

  if (ocad <= EPSILON)
    {
      return FALSE;
    }

  return TRUE;
#else
  gdouble oadc, oadb = (pa->x - pb->x)*(pd->y - pb->y) - (pd->x - pb->x)*(pa->y - pb->y);
  if (oadb >= -EPSILON) {
    return FALSE;
  }

  oadc = (pa->x - pc->x)*(pd->y - pc->y) - (pd->x - pc->x)*(pa->y - pc->y);
  if (oadc <= EPSILON) {
    return FALSE;
  }
  return TRUE;
#endif
}
