#!/usr/bin/env -S deno run --allow-all

/*
This script requires the Deno runtime environment to execute.
Please refer to the official website for instructions on how to install Deno.
https://deno.com/
*/

import { $ } from "https://deno.land/x/dax/mod.ts";
import { fromFileUrl } from "https://deno.land/std/path/mod.ts";

//============================================
console.log("Deno.cwd(): " + Deno.cwd());

const __root_directory = fromFileUrl(new URL("../../", import.meta.url));
Deno.chdir(__root_directory);

console.log("Deno.cwd(): " + Deno.cwd());

const platform = Deno.build.os;

console.log("Deno.build.os: " + platform.toString())

if(platform.toString() === "windows")
{
    await $`${__root_directory}/builds/vs2022/ReactInstrument/ReactInstrument_artefacts/Debug/Standalone/ReactInstrument.exe`;
}
else if(platform.toString() === "darwin")
{
    await $`${__root_directory}/builds/xcode/ReactInstrument/ReactInstrument_artefacts/Debug/Standalone/ReactInstrument.app/Contents/MacOS/ReactInstrument`;
}
else if(platform.toString() === "linux")
{
    await $`${__root_directory}/builds/ninja-single-debug/ReactInstrument/ReactInstrument_artefacts/Debug/Standalone/ReactInstrument`;
}