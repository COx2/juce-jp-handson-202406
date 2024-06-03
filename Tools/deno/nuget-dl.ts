import { download } from "https://deno.land/x/download/mod.ts";

const url =
  "https://dist.nuget.org/win-x86-commandline/latest/nuget.exe";
const fileName = "nuget.exe";
const dir = ".";

await download(url, { file: fileName, dir });
