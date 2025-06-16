if pcall(require, 'lint') then
    require('lint').linters_by_ft = {
        c = {'build_c'},
        cpp = {'build_c'},
    }
    local pattern = [[^([^:]+):(%d+):(%d+):%s+([^:]+):%s+(.*)$]]
    local groups = { 'file', 'lnum', 'col', 'severity', 'message' }
    local severity_map = {
        ['error'] = vim.diagnostic.severity.ERROR,
        ['warning'] = vim.diagnostic.severity.WARN,
        ['performance'] = vim.diagnostic.severity.WARN,
        ['style'] = vim.diagnostic.severity.INFO,
        ['information'] = vim.diagnostic.severity.INFO,
    }

    require('lint').linters.build_c = {
        cmd = 'cc',
        stdin = false,
        append_fname = false,
        args = {'-Wall', '-Wextra', "-fsyntax-only",
            "-Wno-incompatible-pointer-types", "-Wno-override-init",
            "-Wno-unused-variable", "-Wno-unused-parameter",
            "-Wno-unused-function", "-Wno-unused-but-set-variable",
            "-Wno-missing-braces", "src/shaderplay/shaderplay_entry_point.c"
        },
        stream = 'stderr',
        ignore_exitcode = true,
        env = nil,
        parser = require('lint.parser').from_pattern(
            pattern, groups, severity_map, { ['source'] = 'gcc' }
        ),
    }

    vim.api.nvim_create_autocmd({ "BufWritePost" }, {
        callback = function()
            local filename = vim.fn.expand('%:t')
            if string.match(filename, "build.c") then
                require("lint").try_lint('gcc')
            end
        end,
    })
end
