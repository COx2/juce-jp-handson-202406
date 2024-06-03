import { download } from "https://deno.land/x/download/mod.ts";
import { fromFileUrl } from "https://deno.land/std/path/mod.ts";

const __nuget_directory = fromFileUrl(new URL("../../External/nuget", import.meta.url));

const url =
  "https://dist.nuget.org/win-x86-commandline/latest/nuget.exe";
const fileName = "nuget.exe";
const dir = __nuget_directory;

await download(url, { file: fileName, dir });
