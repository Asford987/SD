#!/bin/bash

for file in *; do
	if [[ -f "$file" ]]; then
		newname="$file"
		newname="${newname//1000000000/1B}"
		newname="${newname//100000000/100M}"

		if [[ "$file" != "$newname" ]]; then
			echo "Renomeando: $file -> $newname"
			mv -- "$file" "$newname"
		fi
	fi
done
