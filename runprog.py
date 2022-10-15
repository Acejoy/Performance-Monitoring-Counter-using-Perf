import subprocess
import csv

csv_list = ['l1_read_miss.csv', 'll_read_miss.csv', 'll_write_miss.csv', 'tlb_miss.csv', 'page_fault.csv']


def save_as_csv(label, s):
    # print(s)
    line_list = s.split('\n')
    line_list = list(filter(None, line_list))
    print(line_list)

    for (filename, keyVal) in zip(csv_list, line_list):
        splitText = keyVal.split(' ')
        print(splitText[-1])    
        fields = [label, splitText[-1]]

        with open(filename, 'a') as fd:
            writer = csv.writer(fd)
            writer.writerow(fields)

    return 


matrix_sizes = [2048, 8192]
block_sizes = [8, 16, 32, 64, 128, 256, 512]

# cache_sizes = [256, 512]
# block_sizes = [8, 16]


bin_dict = { 'ijk':'./mat_mul1', 'ikj':'./mat_mul2', 'kij':'./mat_mul3' }
# bin_dict = { 'ijk':'./mat_mul'}

for matrix_size in matrix_sizes:
    for block_size in block_sizes:
        for loopOrder in bin_dict:            
            
            label = loopOrder+'_'+str(matrix_size)+'_'+str(block_size)

            print('=========================running for ',label ,' ======================================')

            value = subprocess.check_output( bin_dict[loopOrder]+' '+str(matrix_size)+' '+str(block_size), shell=True, encoding='UTF-8')
            save_as_csv(label, value)
            print("\n\n================================completed=======================================\n\n")
            # print(value)


# string1 = "initial"
            # s = StringIO(string1)
            # old = sys.stdout

            # sys.stdout = s
            
# f = io.StringIO()
            # with redirect_stdout(f):
            #     os.system(bin_dict[loopOrder]+' '+str(cache_size)+' '+str(block_size))

            # sys.stdout = old
            
            # s = f.getvalue()
            # out = proc.communicate()[0]
            # print(out.upper())
            


            # s.seek(0)
            # print(string1)
            # output=s.read()
            # save_as_csv(label, output)

            # print(":::::",len(s))


# print("running for mat_mul1\n")

# print("running for mat_mul2 \n")
# #os.system(cmd2)
# print("running for mat_mul3 \n")
# #os.system(cmd3)


