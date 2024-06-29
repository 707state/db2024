#!/usr/bin/perl
use strict;
use warnings;
use File::Find;

# 检查是否提供了路径参数
if (@ARGV != 1) {
    die "用法: $0 目录路径\n";
}

# 获取目录路径
my $directory = $ARGV[0];

# 子例程：处理单个文件
sub process_file {
    my $filename = $_;

    # 只处理普通文件
    return unless -f $filename;

    # 打开文件进行读取
    open(my $in, '<', $filename) or die "无法打开文件 $filename: $!";

    # 创建一个临时文件来写入修改后的内容
    open(my $out, '>', "$filename.tmp") or die "无法创建临时文件: $!";

    # 逐行读取文件内容
    while (my $line = <$in>) {
        # 如果行中不包含 "namespace"，则写入临时文件
        print $out $line unless $line =~ /namespace/;
    }

    # 关闭文件句柄
    close($in);
    close($out);

    # 用修改后的临时文件替换原始文件
    rename("$filename.tmp", $filename) or die "无法重命名文件: $!";
}

# 遍历目录并处理所有文件
find(\&process_file, $directory);

print "处理完毕，已删除目录 $directory 中所有文件中包含 'namespace' 的行。\n";

