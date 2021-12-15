#pragma GCC diagnostic ignored "-Wattributes"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <limits>
#include <parted/parted.h>
#include <uuid/uuid.h>
#include <os.h>

using namespace std;
using namespace os_api;

string human_size(uint64_t size){
	stringstream ss;
	if(size < 1024){
		ss << size << " Bytes";
	}else if(size < 1048576){
		ss << size / 1024 << " KB";
	}else if(size < 1073741824){
		ss << size / 1048576 << " MB";
	}else if(size < 1099511627776){
		ss << size / 1073741824 << " GB";
	}else{
		ss << size / 1099511627776 << " TB";
	}
	return ss.str();
}

struct partition_info{
	string path;
	string devicepath;
	string fs;
};


string get_env(const string &name){
	char value[128];
	string ret;
	size_t size=_getenv(name.c_str(), value, 128);
	ret=value;
	if(size>128){
		char *buf=new char[size];
		_getenv(name.c_str(), value, size);
		ret=buf;
	}
	if(size) return ret;
	else return "";
}

bool yesno(string message, char def = '\0'){
	cout << message << endl;
		while(true){
		if(def == 'y'){
			cout << "(Y/n)";
		}else if(def == 'n'){
			cout << "(y/N)";
		}else{
			cout << "(y/n)";
		}
		string line;
		getline(cin, line);
		char c = line[0];
		if(c == 'y' || c == 'Y') return true;
		if(c == 'n' || c == 'N') return false;
		if(c == '\0' && def == 'y') return true;
		if(c == '\0' && def == 'n') return false;
	}
}

partition_info select_partition(){
	map<int, partition_info> selectmap;
	int counter = 0;
	cout << "Please select a partition to install to:" << endl;
	PedDevice *device = NULL;
	ped_device_probe_all();
	while((device = ped_device_get_next(device))){
		cout << device->path << ":" << endl;
		PedDiskType *type = ped_disk_probe(device);
		if(string(type->name) == "msdos"){
			PedDisk *disk = ped_disk_new(device);
			PedPartition *part = ped_disk_next_partition(disk, NULL);
			while(part){
				if(part->num > 0){
					uint64_t size = part->geom.length * device->sector_size;
					stringstream devstream;
					devstream << device->path << "P" << part->num - 1;
					partition_info info;
					info.path = devstream.str();
					info.devicepath = device->path;
					info.fs = ((part->fs_type) ? part->fs_type->name : "unformatted");
					int index = ++counter;
					selectmap[index] = info;
					cout << index << ": " << devstream.str() << ": "<< human_size(size) << " " << ((part->fs_type) ? part->fs_type->name : "unformatted") << endl;
				}
				part = ped_disk_next_partition(disk, part);
			}
		}else{
			cout << "Missing or incompatible partition table." << endl;
		}
	}
	cout <<  endl;
	partition_info ret;
	cout << "Select a partition by number or 'q' to quit (use 'parted' to create/modify partitions)." << endl;
	while(true){
		string line;
		getline(cin, line);
		if(line[0] == 'q') exit(0);
		stringstream ss(line);
		int partno;
		ss >> partno;
		if(partno > 0){
			try{
				ret = selectmap.at(partno);
				break;	
			}catch(out_of_range&){}
		}
	}
	if(ret.fs != "fat16" && ret.fs != "fat32"){
		if(yesno("This partition does not have a compatible filesystem. Format it? (This WILL destroy all data in the partition!)", 'n')){
			_format("FAT", ret.path.c_str(), NULL);
		}else{
			exit(0);
		}
	}else if(yesno("Format partition? (This WILL destroy all data in the partition!)", 'n')){
			_format("FAT", ret.path.c_str(), NULL);
	}
	return ret;
}

string mount_partition(const partition_info &part){
	_mount("hdd", part.path.c_str(), "FAT");
	return "hdd";
}

void install_package(const string &packageFile, const string &dest){
	string sysdrive = get_env("SYSTEMDRIVE");
	char *args[] = {(char*)"install", (char*)packageFile.c_str(), (char*)dest.c_str()};
	string packagePath = sysdrive + ":/os/cmd/package.elx";
	_pid_t pid = _spawn(packagePath.c_str(), 3, args);
	_wait(pid);
}

bool copy_files(const string &mountpoint){
	cout << "Copying files..." << endl;
	string sysdrive = get_env("SYSTEMDRIVE");
	string datapath = sysdrive + ":/packages/base.pkf";
	string kernelpath = sysdrive + ":/packages/kernel.pkf";
	string installpath = sysdrive + ":/packages/install.pkf";
	install_package(datapath, mountpoint + ":/os");
	install_package(kernelpath, mountpoint + ":/os/boot");
	install_package(installpath, mountpoint + ":/os/install");
	return true;
}

void replace_in_string(std::string& str, const std::string& from, const std::string& to) {
    if(from.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}

void replace_in_file(const string &file, const string &placholder, const string &replacement){
	ifstream ifs(file);
	string str;
	ifs.seekg(0, ios::end);
	str.reserve(ifs.tellg());
	ifs.seekg(0, ios::beg);
	str.assign((istreambuf_iterator<char>(ifs)), istreambuf_iterator<char>());
	ifs.close();
	replace_in_string(str, placholder, replacement);
	ofstream ofs(file);
	ofs << str;
}

void make_tar(const string &dir, const string &tar){
	string sysdrive = get_env("SYSTEMDRIVE");
	string tarpath = sysdrive + ":/os/cmd/tar.elx";
	string cwd = get_env("CWD");
	_setenv("CWD", dir.c_str(), 0);
	vector<string> files;
	_handle_t dh = _dopen(dir.c_str(), FS_Read);
	_directory_entry de = _dread(dh);
	while(de.valid){
		files.push_back(de.filename);
		de = _dread(dh);
	}
	_dclose(dh);
	const char *args[files.size() + 2];
	size_t argc = 0;
	args[argc++] = "cvf";
	args[argc++] = tar.c_str();
	for(auto f : files){
		args[argc++] = f.c_str();
	}
	_pid_t pid = _spawn(tarpath.c_str(), argc, (char**)args);
	_wait(pid);
	_setenv("CWD", cwd.c_str(), 0);
}

void configure_install(const string &mountpoint, const partition_info &part){
	cout << "Configuring install..." << endl;
	string configpath = mountpoint + ":/os/boot/initfs/config.ini";
	replace_in_file(configpath, "$DRIVE$", mountpoint);
	replace_in_file(configpath, "$DEVICE$", part.path);
	string inittar = mountpoint + ":/os/boot/init.tar";
	string initfspath = mountpoint + ":/os/boot/initfs";
	cout << "Building init.tar..." << endl;
	make_tar(initfspath, inittar);
}

bool install_grub(const string &mountpoint, const partition_info &part){
	cout << "Installing GRUB (bootloader)..." << endl;
	string loadcfgpath = mountpoint + ":/os/boot/grub/load_cfg";
	uuid_t uuid;
	uuid_generate(uuid);
	char uuidc[64];
	uuid_unparse_upper(uuid, uuidc);
	string uuidfile = mountpoint + ":/os/boot/" + uuidc + ".id";
	ofstream ofs(uuidfile);
	ofs << "This file identifies this installation during boot." << endl;
	ofs.close();
	replace_in_file(loadcfgpath, "$UUID$", uuidc);
	string mkim = mountpoint + ":/os/boot/grub/grubmkim.elx";
	string cwd = get_env("CWD");
	_setenv("CWD", (mountpoint + ":/").c_str(), 0);
	const char *mkimargs[] = {"-O", "i386-pc", "-d", "/os/boot/grub/i386-pc", "--prefix=/os/boot/grub/i386-pc", "-o", "/os/boot/grub/i386-pc/core.img", "biosdisk", "msdospart", "part_msdos", "fat", "search", "configfile", "multiboot", "-c", loadcfgpath.c_str()};
	_pid_t mkimpid = _spawn(mkim.c_str(), sizeof(mkimargs)/sizeof(mkimargs[0]), (char**)mkimargs);
	_wait(mkimpid);
	string bios = mountpoint + ":/os/boot/grub/grubbios.elx";
	const char *biosargs1[] = {"-vv", "-d", "/os/boot/grub/i386-pc", part.path.c_str()};
	_pid_t biospid1 = _spawn(bios.c_str(), 4, (char**)biosargs1);
	_wait(biospid1);
	if(yesno("Do you want to install GRUB to this drive's MBR? (Choose 'yes' unless you have another bootloader that you can configure.)", 'y')){
		const char *biosargs2[] = {"-vv", "-d", "/os/boot/grub/i386-pc", part.devicepath.c_str()};
		_pid_t biospid2 = _spawn(bios.c_str(), 4, (char**)biosargs2);
		_wait(biospid2);
	}
	_setenv("CWD", cwd.c_str(), 0);
	return true;
}

void write_answers(const string &mountpoint, const partition_info &part, bool rootinstall){
	string setuppath = mountpoint + ":/os/install/setup.ini";
	replace_in_file(setuppath, "$DEVICE$", part.devicepath);
	replace_in_file(setuppath, "$PART$", part.path);
	replace_in_file(setuppath, "$FS$", part.fs);
	replace_in_file(setuppath, "$DRIVE$", mountpoint);
	replace_in_file(setuppath, "$MBR$", rootinstall ? "true" : "false");
}

_pid_t setup_registry(){
	string sysdrive = get_env("SYSTEMDRIVE");
	string regCmd = sysdrive + ":/os/system/registry.elx";
	const char *regargs[] = {"-f", ":memory:"};
	_pid_t regpid = _spawn(regCmd.c_str(), sizeof(regargs)/sizeof(regargs[0]), (char**)regargs);
	char *regpid_str;
	asprintf(&regpid_str, "%i", (int)regpid);
	_setenv("REGISTRY_PID", regpid_str, 0);
	free(regpid_str);
	return regpid;
}

void finalise_registry(const string &mountpoint, _pid_t registry_pid){
	string regCmd = mountpoint + ":/os/cmd/reg.elx";
	string regPath = mountpoint + ":/os/config/registry.db";
	const char *regargs[] = {"backup", regPath.c_str()};
	_pid_t regpid = _spawn(regCmd.c_str(), sizeof(regargs)/sizeof(regargs[0]), (char**)regargs);
	_wait(regpid);
	_kill(registry_pid);
}

int main(){
    cout << "OS Installer" << endl << endl;
	partition_info partition = select_partition();
	string mountpoint = mount_partition(partition);
	_pid_t regpid = setup_registry();
	if(copy_files(mountpoint)){
		configure_install(mountpoint, partition);
		bool rootinstall = install_grub(mountpoint, partition);
		write_answers(mountpoint, partition, rootinstall);
		finalise_registry(mountpoint, regpid);
	}else{
		_kill(regpid);
	}
	
    return 0;
}