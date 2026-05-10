# The __init__.py registers the Node in ComfyUI.
# Copyright (C) 2026  spectre-bit
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

from .build import SpectreNodes

NODE_CLASS_MAPPINGS = {
    "SpectreNodesSeedXoshiro256**": SpectreNodes.SeedNode
}

NODE_DISPLAY_NAME_MAPPINGS = {
    "SpectreNodesSeedXoshiro256**": "Seed (Xoshiro256**)"
}

WEB_DIRECTORY = "./web"

__all__ = ["NODE_CLASS_MAPPINGS", "NODE_DISPLAY_NAME_MAPPINGS", "WEB_DIRECTORY"]