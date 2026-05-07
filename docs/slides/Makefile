# Render the XIP-cache slide-deck PNGs from their SVG sources.
#
# Sources are hand-written SVG (cleaner output than auto-routed
# Graphviz: full control over text wrap, label positions, and
# colours).  ImageMagick (`convert`) rasterises them to 144-DPI PNGs
# for embedding in slide tools that don't render SVG natively.
#
# Run:
#     make            # render every *.svg to *.png
#     make clean      # delete the PNGs (keep SVGs in source control)
#
# Requires: ImageMagick (`convert`).  The committed PNGs let the deck
# render correctly on GitHub even without that toolchain.

SVG_FILES := $(wildcard *.svg)
PNG_FILES := $(SVG_FILES:.svg=.png)

DPI ?= 144

.PHONY: all clean
all: $(PNG_FILES)

%.png: %.svg
	convert -density $(DPI) -background white $< $@

clean:
	rm -f $(PNG_FILES)
