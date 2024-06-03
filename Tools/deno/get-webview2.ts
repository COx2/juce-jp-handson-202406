import { $ } from "https://deno.land/x/dax/mod.ts";
import { fromFileUrl } from "https://deno.land/std/path/mod.ts";

const __nuget_directory = fromFileUrl(new URL("../../External/nuget", import.meta.url));
Deno.chdir(__nuget_directory);
console.log("Deno.cwd(): " + Deno.cwd());

await $`.\\nuget.exe install Microsoft.Web.WebView2`;
