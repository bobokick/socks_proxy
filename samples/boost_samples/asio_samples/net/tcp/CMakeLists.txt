
# 设置需要编译的子文件夹
set(COMPILE_FOLDERS
    client
    server
)

#####################################################################以下不需要进行手动设置###########################################################################

# 需要编译的子项目。
foreach(folder ${COMPILE_FOLDERS})
    add_subdirectory("${folder}")
endforeach()
