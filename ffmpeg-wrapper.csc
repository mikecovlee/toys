@charset: utf8

import imgui_gl2     as gui
import imgui_font    as font
import async.future  as future
import async.process as process
import regex

system.file.remove("." + system.path.separator + "imgui.ini")
system.file.mkdir("." + system.path.separator + "output")

class ffmpeg_options
    var actual_name = ""
    var value = ""
end

var options = new hash_map

function add_option(name, actual_name)
    var o = new ffmpeg_options
    o.actual_name = actual_name
    options.insert(name, o)
end

# in bits/s
add_option("quality", "-b:v")
add_option("framerate", "-r")
add_option("codec", "-c:v")

var builder = new process.builder
builder.redirect_stdin(false)
builder.redirect_stdout(true)

var current_dir = runtime.get_current_dir()
var ffmpeg_bin = current_dir + system.path.separator + "bin" + system.path.separator + "ffmpeg.exe"

var path = "."
var filters = {".*\\.avi", ".*\\.flv", ".*\\.mp4"}
var path_name = null

function read_path()
    if path[path.size()-1] != system.path.separator
        path += system.path.separator
    end
    var path_info = new array
    path_name = new array
    var info = system.path.scan(path)
    var regs = new array
    var files = new array
    foreach it in filters
        regs.push_back(regex.build(it))
    end
    foreach it in info
        if it.type()==system.path.type.dir
            path_info.push_back(it)
        else
            foreach reg in regs
                if !reg.match(it.name()).empty()
                    files.push_back(it)
                end
            end
        end
    end
    foreach it in files
        path_info.push_back(it)
    end
    foreach it in path_info
        if it.type() != system.path.type.dir
            path_name.push_back(it.name())
        end
    end
end

read_path()

var app = gui.window_application(400, 600, "Video Compression Utility by @mikecovlee")
var cn_font = gui.add_font_extend_cn(font.source_han_sans, 18)
var win_opened = true
var popup1_opened = true
var popup2_opened = true
var framerate_input = 24
var quality_input = 1000
var suffix_input = 0
var codec_input = 1
var selected = -1

function set_default()
    framerate_input = 24
    quality_input = 1000
    suffix_input = 0
    codec_input = 1
end

var ffmpeg_process = null
var output_suffix = null
var file_index = -1
var state = 0

function start_compression()
    if suffix_input == 1 && codec_input != 1
        gui.open_popup("提示##2")
        popup2_opened = true
        return
    end
    options.quality.value = to_string(to_integer(quality_input*1000))
    options.framerate.value = to_string(to_integer(framerate_input))
    switch suffix_input
        case 0
            output_suffix = ".avi"
        end
        case 1
            output_suffix = ".flv"
        end
        case 2
            output_suffix = ".mp4"
        end
    end
    switch codec_input
        case 0
            options.codec.value = "mpeg4"
        end
        case 1
            options.codec.value = "h264"
        end
        case 2
            options.codec.value = "hevc"
        end
    end
    file_index = -1
    state = 1
end

function get_filename(name)
    while name[-1] != '.'
        name.cut(1)
    end
    name.cut(1)
    return name
end

function stop_compression()
    if ffmpeg_process != null && !ffmpeg_process.has_exited()
        ffmpeg_process.kill()
        ffmpeg_process.wait()
        ffmpeg_process = null
    end
end

set_default()

loop
    app.prepare()
    gui.push_font(cn_font)
    gui.style_color_light()
    gui.begin_window("视频压缩实用程序 by @mikecovlee", win_opened, {gui.flags.no_resize, gui.flags.no_move, gui.flags.no_collapse})
        if gui.begin_popup_modal("提示##1", popup1_opened, {gui.flags.no_move, gui.flags.always_auto_resize})
            gui.text("转码结束")
            if gui.button("OK")
                gui.close_current_popup()
            end
            gui.end_popup()
        end
        if gui.begin_popup_modal("提示##2", popup2_opened, {gui.flags.no_move, gui.flags.always_auto_resize})
            gui.text("flv格式仅支持H.264编码器")
            if gui.button("OK")
                gui.close_current_popup()
            end
            gui.end_popup()
        end
        gui.set_window_pos(gui.vec2(0, 0))
        gui.set_window_size(gui.vec2(app.get_window_width(), app.get_window_height()))
        if state != 1
            gui.progress_bar(0, "就绪，点击“开始转码”按钮开始转码")
            gui.separator()
            gui.list_box("输入文件列表", selected, path_name)
            if gui.button("开始转码")
                start_compression()
            end
            gui.same_line()
            if gui.button("刷新列表")
                read_path()
            end
            gui.same_line()
            if gui.button("恢复默认")
                set_default()
            end
            gui.same_line()
            if gui.button("退出程序")
                break
            end
            gui.separator()
            gui.text("目标帧率(越高文件越大, 画面越流畅)")
            gui.slider_float("单位: 帧/秒", framerate_input, 24, 60)
            gui.separator()
            gui.text("目标比特率(越高文件越大, 质量越好)")
            gui.slider_float("单位: 千比特/秒", quality_input, 200, 10000)
            gui.separator()
            gui.text("输出文件格式(默认: .avi)")
            gui.radio_button(".mp4(慢，体积一般，画质最好)", suffix_input, 2)
            gui.radio_button(".avi(中，体积最大，画质一般)", suffix_input, 0)
            gui.radio_button(".flv(快，体积最小，画质最差)", suffix_input, 1)
            gui.separator()
            gui.text("编码器(默认: H.264)")
            gui.radio_button("MPEG4(兼容性最好，压缩率最低)", codec_input, 0)
            gui.radio_button("H.264(兼容性一般，压缩率一般)", codec_input, 1)
            gui.radio_button("HEVC(兼容性较差，压缩率最高)", codec_input, 2)
        else
            if ffmpeg_process == null || ffmpeg_process.has_exited()
                ++file_index
                if file_index < path_name.size()
                    var opt = "-i " + path_name[file_index] + " "
                    foreach it in options
                        opt += it.second().actual_name + " " + it.second().value + " -y "
                    end
                    ffmpeg_process = builder.start(ffmpeg_bin, opt + "." + system.path.separator + "output" + system.path.separator + get_filename(clone(path_name[file_index])) + output_suffix)    
                else
                    ffmpeg_process = null
                    gui.open_popup("提示##1")
                    popup1_opened = true
                    state = 0
                end
            end
            gui.progress_bar(file_index / path_name.size(), "正在转码...请稍候")
            if gui.button("停止转码")
                stop_compression()
                state = 0
            end
            if gui.button("退出程序")
                break
            end
        end
        gui.separator()
        gui.text("版权所有 © 智锐科创计算机协会 2015 - 2020")
    gui.end_window()
    gui.pop_font()
    app.render()
until !win_opened || app.is_closed()
stop_compression()