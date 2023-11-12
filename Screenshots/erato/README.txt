erato_LOD0lightmap_LOD_i:
	These lightmaps are all using LOD0's lightmap baked at
	resolution 256.

erato_uniquelightmaps_LOD_i:
	These lightmaps are all using their own LOD's lightmap
	baked at resolution 256.

Unreal has the option to enforce a "min lightmap size", which is
	what's usually done for stuff like this because smaller
	LOD's don't need such high lightmap resolutions.
	Comparisons for those are in the folder "Screenshots/erato_smallerlightmaps"