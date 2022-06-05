#!/usr/bin/env bash

sed -E 's/<!-- pandoc (.*?) -->/\1/g; s/<br( )?(\/)?>/ \\newline /g; s/<(\/)?pre>//g' <README.md >README.pandoc.md
pandoc README.pandoc.md -o README.pdf
