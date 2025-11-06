if (vim.fn.has('win32') == 1 or vim.fn.has('win64') == 1) then
    vim.opt.makeprg="setup_x64.bat && cl.exe build.c -nologo -Z7 -Fo:build\\ -Fe:build\\build.exe && build\\build.exe build-run --nocolor"
else
    vim.opt.makeprg="cc -ggdb build.c && ./a.out build-run --nocolor"
end
