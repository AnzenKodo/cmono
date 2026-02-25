local build_command = ""
local cc_command = ""
if (vim.fn.has('win32') == 1 or vim.fn.has('win64') == 1) then
    build_command="setup_x64.bat && cl.exe build.c -nologo -Z7 -Fo:build\\ -Fe:build\\build.exe"
    cc_command=build_command.." && build\\build.exe build-"
else
    build_command="cc -ggdb build.c"
    cc_command=build_command.." && ./a.out build-"
end
vim.opt.makeprg=cc_command .. "run --nocolor"
vim.api.nvim_create_user_command("Build",  function()
    vim.opt.makeprg=build_command
    vim.cmd('make')
    vim.opt.makeprg=cc_command .. "run --nocolor"
end, { desc = "Bootstrap build system"})

-- Setup Termdebug ============================================================

local termdebug_config = vim.g.termdebug_config or {}
termdebug_config.command = {
    "gdb", "-nx",
    "-ex", "set breakpoint pending on",
    "-ex", "set disassembly-flavor intel",
    "-ex", "set confirm off",
    -- "-ex", "tbreak entry_point",
    "./build/cope_debug"
}
vim.g.termdebug_config = termdebug_config

vim.api.nvim_create_autocmd("User", {
    pattern = "TermdebugStartPost",
    callback = function()
        vim.keymap.set("n", "<leader>dr",  ":call TermDebugSendCommand('dr')<CR>", { desc = "[d]ebugger [r]un (modify)" })
        vim.cmd('cexpr system("' .. cc_command .. 'debugger --nocolor")')
        vim.fn.TermDebugSendCommand('define dr')
        vim.fn.TermDebugSendCommand('   shell cc build.c && ./a.out build-debugger')
        vim.fn.TermDebugSendCommand('   file build/cope_debug')
        vim.fn.TermDebugSendCommand('   run')
        vim.fn.TermDebugSendCommand('end')
    end,
})
