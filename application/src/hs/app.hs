{-
   Antiflood Copyright (C) 2019 Alexey <Inverse> Shumeiko

   This file is part of Antiflood project.

   This firmware is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This firmware is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with the firmware. If not, see <http://www.gnu.org/licenses/>.

   For more details see LICENSE file.
-}

import Graphics.Gloss

-- | Display the last event received as text.
main
 = play (InWindow "Tester" (700, 100) (10, 10))
        white
        100
        ""
        (\str     -> Translate (-340) 0 $ Scale 0.1 0.1 $ Text str)
        (\event _ -> show event)
        (\_ world -> world)
