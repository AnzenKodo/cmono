-- TODO(ak): Add every build user command and debug also make helper functions
-- for making them.
-- TODO(ak): replace build command variables with single function

-- ak: Build Initialization
-- ============================================================================

local build_dir = "build"
local build_command = ""
local cc_command = ""
if (vim.fn.has('win32') == 1 or vim.fn.has('win64') == 1) then
    build_command = "setup_x64.bat && cl.exe build.c -nologo -Z7 -Fo:build\\ -Fe:"..build_dir.."\\build.exe"
    cc_command = build_command.." && "..build_dir.."\\build.exe "
else
    build_command = "cc -ggdb build.c"
    cc_command = build_command.." && ./a.out "
end

-- ak: Build user commands
-- ============================================================================

-- NOTE(ak): run with `:make` below command
vim.opt.makeprg = cc_command .. "build-run --nocolor"
vim.api.nvim_create_user_command("Build",  function()
    vim.opt.makeprg = build_command
    vim.cmd('make')
    vim.opt.makeprg = cc_command .. "build-run --nocolor"
end, { desc = "Bootstrap build system"})
vim.api.nvim_create_user_command("Meta",  function()
    vim.opt.makeprg = cc_command .. "gen-meta --nocolor"
    vim.cmd('make')
    vim.opt.makeprg = cc_command .. "build-run --nocolor"
end, { desc = "Bootstrap build system"})

-- ak: Setup Termdebug
-- ============================================================================

local program_name_debug = build_dir.."/cope_debug"
local termdebug_config = vim.g.termdebug_config or {}
termdebug_config.command = {
    "gdb", "-nx",
    "-ex", "set breakpoint pending on",
    "-ex", "set disassembly-flavor intel",
    "-ex", "set confirm off",
    "-ex", "set print pretty on",
    program_name_debug
}
vim.g.termdebug_config = termdebug_config

vim.api.nvim_create_autocmd("User", {
    pattern = "TermdebugStartPost",
    callback = function()
        vim.keymap.set("n", "<leader>dr", ":call TermDebugSendCommand('dr')<CR>", { desc = "[d]ebugger [r]un" })
        vim.cmd('cexpr system("' .. cc_command .. 'build-debugger --nocolor")')
        vim.fn.TermDebugSendCommand('define dr')
        vim.fn.TermDebugSendCommand('   shell '..cc_command.."build-debugger")
        vim.fn.TermDebugSendCommand('   file '..program_name_debug)
        vim.fn.TermDebugSendCommand('   run')
        vim.fn.TermDebugSendCommand('end')
    end,
})
