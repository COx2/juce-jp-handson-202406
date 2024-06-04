import { $ } from "https://deno.land/x/dax/mod.ts";
import { fromFileUrl } from "https://deno.land/std/path/mod.ts";

const __sp_directory = fromFileUrl(new URL("../../SamplePack", import.meta.url));
Deno.chdir(__sp_directory);
console.log("Deno.cwd(): " + Deno.cwd());

await $`git clone https://github.com/TheSmallTeaBoi/the-libre-sample-pack`;
