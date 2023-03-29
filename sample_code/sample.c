print_locals 0x7f8cff81f0 0x7f8cff83e0 0x55668886e4


7f8cff81d0, 7f8cff83e0
0x556683d1f0 is in operator()() const (/home/xuhongjin/A-GI/CI_Script/src/APP/InitConfigCode/output/code/VirtualTesterAPP/src/DM_DiagClientInterf.cpp:910).905  /home/xuhongjin/A-GI/CI_Script/src/APP/InitConfigCode/output/code/VirtualTesterAPP/src/DM_DiagClientInterf.cpp: No such file or directory.
7f8cff81d0, 7f8cff8830


专家资源并不是无限的，而是受到时间、精力、成本等因素的限制
我们在使用专家资源时，应该明确目标和需求，尊重专家的意见和建议，适度使用专家资源，以达到最佳的效果。


dlt_user_housekeeperthread_function  0x7fa02c06bc c6bc





#include <stdio.h>
#include <stdlib.h>
#include <dwarf.h>
#include <libdwarf.h>

// 一个用于打印错误信息的函数
void print_error(Dwarf_Debug dbg, const char *msg, int res, Dwarf_Error err) {
    printf("%s ERROR: %s (%d) %s\n", msg, dwarf_errmsg(err), res, dwarf_errname(res));
    if (dbg != NULL) {
        dwarf_dealloc(dbg, err, DW_DLA_ERROR);
    }
    exit(1);
}

// 一个用于打印变量类型和取值的函数
void print_variable(Dwarf_Debug dbg, Dwarf_Attribute attr, Dwarf_Addr addr) {
    Dwarf_Error err;
    Dwarf_Half form;
    Dwarf_Locdesc *locdesc;
    Dwarf_Signed loccnt;
    int res;

    // 获取属性的表现形式
    res = dwarf_whatform(attr, &form, &err);
    if (res != DW_DLV_OK) {
        print_error(dbg, "dwarf_whatform", res, err);
    }

    // 如果属性是一个位置表达式
    if (form == DW_FORM_exprloc || form == DW_FORM_block1 || form == DW_FORM_block2 || form == DW_FORM_block4 || form == DW_FORM_block) {
        // 获取位置描述
        res = dwarf_loclist_n(attr, &locdesc, &loccnt, &err);
        if (res != DW_DLV_OK) {
            print_error(dbg, "dwarf_loclist_n", res, err);
        }

        // 如果只有一个位置描述
        if (loccnt == 1) {
            // 获取位置操作数
            Dwarf_Loc *loc = locdesc->ld_s;
            Dwarf_Small op = loc->lr_atom;

            // 如果操作数是一个偏移量
            if (op == DW_OP_fbreg) {
                // 获取偏移量的值
                Dwarf_Signed offset = loc->lr_number;

                // 计算变量的地址
                Dwarf_Addr var_addr = addr + offset;

                // 打印变量的地址和取值
                printf("Variable address: 0x%llx\n", var_addr);
                printf("Variable value: %d\n", *(int *)var_addr); // 假设变量是int类型

                // 释放位置描述
                dwarf_dealloc(dbg, locdesc->ld_s, DW_DLA_LOC_BLOCK);
                dwarf_dealloc(dbg, locdesc, DW_DLA_LOCDESC);
            }
        }
    }

    // 释放属性
    dwarf_dealloc(dbg, attr, DW_DLA_ATTR);
}

// 一个用于获取变量类型的函数
void get_variable_type(Dwarf_Debug dbg, Dwarf_Die die) {
    Dwarf_Error err;
    Dwarf_Attribute attr;
    Dwarf_Half tag;
    int res;

    // 获取DIE的标签
    res = dwarf_tag(die, &tag, &err);
    if (res != DW_DLV_OK) {
        print_error(dbg, "dwarf_tag", res, err);
    }

    // 如果标签是一个基本类型
    if (tag == DW_TAG_base_type) {
        char *name;

        // 获取类型的名称属性
        res = dwarf_diename(die, &name, &err);
        if (res != DW_DLV_OK) {
            print_error(dbg, "dwarf_diename", res, err);
        }

        // 打印类型的名称
        printf("Variable type: %s\n", name);

        // 释放名称字符串
        dwarf_dealloc(dbg, name, DW_DLA_STRING);
    }

    // 如果标签是一个指针类型或者引用类型
    if (tag == DW_TAG_pointer_type || tag == DW_TAG_reference_type){
        // 获取类型的类型属性
        res = dwarf_attr(die, DW_AT_type, &attr, &err);
        if (res != DW_DLV_OK) {
            print_error(dbg, "dwarf_attr", res, err);
        }

        // 获取类型的DIE
        Dwarf_Die type_die;
        res = dwarf_formref_die(attr, &type_die, &err);
        if (res != DW_DLV_OK) {
            print_error(dbg, "dwarf_formref_die", res, err);
        }

        // 递归获取类型的类型
        get_variable_type(dbg, type_die);

        // 释放属性和DIE
        dwarf_dealloc(dbg, attr, DW_DLA_ATTR);
        dwarf_dealloc(dbg, type_die, DW_DLA_DIE);
    }
}

// 一个用于遍历DIE树的函数
void traverse_die_tree(Dwarf_Debug dbg, Dwarf_Die die) {
    Dwarf_Error err;
    Dwarf_Half tag;
    int res;

    // 获取DIE的标签
    res = dwarf_tag(die, &tag, &err);
    if (res != DW_DLV_OK) {
        print_error(dbg, "dwarf_tag", res, err);
    }

    // 如果标签是一个变量
    if (tag == DW_TAG_variable) {
        char *name;

        // 获取变量的名称属性
        res = dwarf_diename(die, &name, &err);
        if (res != DW_DLV_OK) {
            print_error(dbg, "dwarf_diename", res, err);
        }

        // 打印变量的名称
        printf("Variable name: %s\n", name);

        // 释放名称字符串
        dwarf_dealloc(dbg, name, DW_DLA_STRING);

        // 获取变量的位置属性
        Dwarf_Attribute attr;
        res = dwarf_attr(die, DW_AT_location, &attr, &err);
        if (res == DW_DLV_OK) {
            // 假设变量的地址是0x1000，可以根据实际情况修改
            print_variable(dbg, attr, 0x1000);
        }

        // 获取变量的类型属性
        res = dwarf_attr(die, DW_AT_type, &attr, &err);
        if (res == DW_DLV_OK) {
            // 获取变量的类型DIE
            Dwarf_Die type_die;
            res = dwarf_formref_die(attr, &type_die, &err);
            if (res != DW_DLV_OK) {
                print_error(dbg, "dwarf_formref_die", res, err);
            }

            // 获取变量的类型
            get_variable_type(dbg, type_die);

            // 释放属性和DIE
            dwarf_dealloc(dbg, attr, DW_DLA_ATTR);
            dwarf_dealloc(dbg, type_die, DW_DLA_DIE);
        }
    }

    // 遍历DIE的子节点
    Dwarf_Die child;
    res = dwarf_child(die, &child, &err);
    if (res == DW_DLV_OK) {
        traverse_die_tree(dbg, child);

        // 释放子节点DIE
        dwarf_dealloc(dbg, child, DW_DLA_DIE);
    }

    // 遍历DIE的兄弟节点
    Dwarf_Die sibling;
    res = dwarf_siblingof_b(dbg, die, 1 /* is_info */, &sibling,
                            &err);
    if (res == DW_DLV_OK) {
        traverse_die_tree(dbg, sibling);

        // 释放兄弟节点DIE
        dwarf_dealloc(dbg, sibling, DW_DLA_DIE);
    }
}

// 一个用于读取编译单元列表的函数
void read_cu_list(Dwarf_Debug dbg) {
    Dwarf_Error err;
    Dwarf_Unsigned cu_header_length = 0;
    Dwarf_Half version_stamp = 0;
    Dwarf_Unsigned abbrev_offset = 0;
    Dwarf_Half address_size = 0;
    Dwarf_Unsigned next_cu_header = 0;
    Dwarf_Half header_cu_type = DW_UT_compile; // 假设是编译单元类型
    int res;

    // 遍历所有的编译单元头
    for (;;) {
        // 获取编译单元头信息
        res = dwarf_next_cu_header_c(dbg, 1 /* is_info */, &cu_header_length,
                                     &version_stamp, &abbrev_offset,
                                     &address_size, &header_cu_type,
                                     &next_cu_header, &err);
        if (res == DW_DLV_ERROR) {
            print_error(dbg, "dwarf_next_cu_header", res, err);
        }
        if (res == DW_DLV_NO_ENTRY) {
            // 没有更多的编译单元头，退出循环
            break;
        }

        // 获取编译单元的根DIE
        Dwarf_Die cu_die = NULL;
        res = dwarf_siblingof_b(dbg, NULL /* in_die */, 1 /* is_info */,
                                &cu_die, &err);
        if (res == DW_DLV_ERROR) {
            print_error(dbg, "dwarf_siblingof_b", res, err);
        }
        if (res == DW_DLV_NO_ENTRY) {
            // 没有根DIE，跳过这个编译单元
            continue;
        }

        // 遍历编译单元的DIE树
        traverse_die_tree(dbg, cu_die);

        // 释放根DIE
        dwarf_dealloc(dbg, cu_die, DW_DLA_DIE);
    }
}

// 主函数，打开文件，初始化libdwarf，调用read_cu_list，关闭文件
int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <file>\n", argv[0]);
        exit(1);
    }

    char *file_name = argv[1];
    int fd = open(file_name, O_RDONLY);
    if (fd < 0) {
        printf("Failed to open file %s\n", file_name);
        exit(1);
    }

    Dwarf_Debug dbg = NULL;
    Dwarf_Error err;
    int res = dwarf_init(fd, DW_DLC_READ, NULL /* errhand */, NULL /* errarg */, &dbg, &err);
    if (res != DW_DLV_OK) {
        printf("Failed to initialize libdwarf\n");
        exit(1);
    }

    read_cu_list(dbg);

    res = dwarf_finish(dbg, &err);
    if (res != DW_DLV_OK) {
        printf("Failed to finish libdwarf\n");
        exit(1);
    }

    close(fd);
    return 0;
}
