#!/usr/bin/perl
use strict;
use warnings;

# 确定要处理的文件名
my $filename = $ARGV[0];

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

print "处理完毕，已删除包含 'namespace' 的行。\n";

