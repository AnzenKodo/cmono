if (vim.fn.has('win32') == 1 or vim.fn.has('win64') == 1) then
    vim.opt.makeprg="a.exe build-run"
else
    vim.opt.makeprg="cc -ggdb build.c && ./a.out build-run mingw"
end
