#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <stdexcept>
#include <sstream>
#include <cmath>
#include <cstdlib> 

using namespace std;

// --- MA MAU ANSI CHO GIAO DIEN CONSOLE ---
#define RESET   "\033[0m"
#define RED     "\033[31m"      // Lỗi, cảnh báo
#define GREEN   "\033[32m"      // Thành công, OK
#define YELLOW  "\033[33m"      // Nhắc nhở, nhập liệu
#define BLUE    "\033[34m"      // Tiêu đề lớn
#define CYAN    "\033[36m"      // Tiêu đề phụ, khung bảng
#define MAGENTA "\033[35m"      // Điểm nhấn, MaVe

// --- LOP NGOAI LE (CUSTOM EXCEPTION) ---
class InputException : public runtime_error {
public:
    InputException(string msg) : runtime_error(msg) { }
};

// --- CAC HAM KIEM TRA DU LIEU ---
bool laSdtVietNam(string s) {
    if (s.length() != 10) return false;
    for (char c : s) if (!isdigit(c)) return false;
    return s[0] == '0' && (s[1] == '3' || s[1] == '5' || s[1] == '7' || s[1] == '8' || s[1] == '9');
}

bool laTenHopLe(string s) {
    if (s.empty()) return false;
    for (char c : s) {
        if (!isalpha(c) && !isspace(c)) return false;
    }
    return true;
}

// --- CAU TRUC DIEM DUNG TREN QL20 (DA THONG KE CHI TIET CAC TRAM) ---
struct TramDung {
    string tenTram;
    int kmTuHCM; 
};

const vector<TramDung> danhSachTram = {
    {"TP.HCM", 0}, 
    {"DN Dau Giay", 40}, 
    {"DN Dinh Quan", 65},
    {"DN Tan Phu", 80}, 
    {"DH Ma Da Guoi", 120},
    {"DH Deo Chuoi", 140}, 
    {"DH Da Huoai", 155},
    {"BL Bao Loc", 175},
    {"BL Trung Tam", 180}, 
    {"BL Bao Lam", 195}, 
    {"DL Di Linh", 220},
    {"DL Gia Hiep", 235},
    {"DL Trung Tam", 245}, 
    {"DT Duc Trong", 260},
    {"DT Lien Nghia", 270}, 
    {"DT Phi Nom", 285}, 
    {"DA LAT", 300}
};

// --- CAU TRUC XE ---
struct ThongTinXe {
    string bienSo;
    string loaiXe;
    int tongGhe;
    int donGia;
    vector<int> gheTrong; 
    int maXichChay;       

    ThongTinXe(string bs, string lx, int tg, int dg, vector<int> gt, int mxc = -1)
        : bienSo(bs), loaiXe(lx), tongGhe(tg), donGia(dg), gheTrong(gt), maXichChay(mxc) {}
};

// Kho 10 xe dung chung toan he thong
vector<ThongTinXe> khoXe = {
    {"51B-101.01", "Ghe ngoi", 16, 1500, {16,16,16,16}, -1},
    {"51B-102.02", "Ghe ngoi", 16, 1500, {16,16,16,16}, -1},
    {"50F-201.01", "Ghe ngoi", 29, 1500, {29,29,29,29}, -1},
    {"50F-202.02", "Ghe ngoi", 29, 1500, {29,29,29,29}, -1},
    {"59G-301.01", "Ghe ngoi", 45, 1500, {45,45,45,45}, -1},
    {"59G-302.02", "Ghe ngoi", 45, 1500, {45,45,45,45}, -1},
    
    {"51B-401.01", "Giuong nam", 34, 3000, {34,34,34,34}, -1},
    {"51B-402.02", "Giuong nam", 34, 3000, {34,34,34,34}, -1},
    {"50E-501.01", "Giuong nam", 40, 3000, {40,40,40,40}, -1},
    {"50E-502.02", "Giuong nam", 40, 3000, {40,40,40,40}, -1}
};

// --- LOP NGUOI ---
class Nguoi {
protected:
    string hoTen;
public:
    virtual void nhap() = 0;
    virtual void xuat() = 0;
    virtual string getHoTen() { return hoTen; }
    virtual string getSoDienThoai() = 0;
    virtual void setHoTen(string t) { hoTen = t; }
    virtual ~Nguoi() {}
};

class KhachHang : public Nguoi {
private:
    string soDienThoai;
public:
    void nhap() override {
        cout << CYAN << "\n   [ BUOC 1: THONG TIN KHACH HANG ]\n" << RESET;
        
        while (true) {
            try {
                cout << YELLOW << "   + Ho va ten (Khong dau): " << RESET; getline(cin, hoTen);
                if (!laTenHopLe(hoTen)) {
                    throw InputException("Ten khong hop le (chi dung chu cai va khoang trang).");
                }
                break; 
            } catch (const InputException& e) {
                cout << RED << "   [!] LOI: " << e.what() << " Vui long nhap lai!\n" << RESET;
            }
        }
        
        while (true) {
            try {
                cout << YELLOW << "   + So dien thoai VN: " << RESET; getline(cin, soDienThoai);
                if (!laSdtVietNam(soDienThoai)) {
                    throw InputException("SDT phai dung 10 chu so va thuoc nha mang VN (03/05/07/08/09).");
                }
                break; 
            } catch (const InputException& e) {
                cout << RED << "   [!] LOI: " << e.what() << " Vui long nhap lai!\n" << RESET;
            }
        }
    }
    void xuat() override { cout << left << setw(16) << hoTen << " | " << setw(11) << soDienThoai; }
    string getSoDienThoai() override { return soDienThoai; }
    void setSoDienThoai(string s) { soDienThoai = s; }
};

// --- LOP CHUYEN XE ---
class ChuyenXe {
private:
    string maChuyen, diemDi, diemDen, loaiDuong, diemDon, diemTra, gioDonXe;
    int indexTuyen, indexGio, khoangCach, indexXe, soLuongVe;
public:
    ChuyenXe() { indexXe = 0; soLuongVe = 0; khoangCach = 0; indexTuyen = 0; indexGio = 0; }

    void nhap() {
        int chonTuyen, chonGio;
        cout << CYAN << "\n   [ BUOC 2: CHON TUYEN DUONG VA GIO CHAY ]\n" << RESET;
        
        while (true) {
            cout << "   1. Tuyen: TP.HCM -> DA LAT\n";
            cout << "   2. Tuyen: DA LAT -> TP.HCM\n";
            cout << YELLOW << "   => Lua chon tuyen (1-2): " << RESET;
            if (cin >> chonTuyen && (chonTuyen == 1 || chonTuyen == 2)) break;
            cout << RED << "   [!] Lua chon khong hop le. Nhap lai!\n" << RESET;
            cin.clear(); cin.ignore(1000, '\n');
        }
        indexTuyen = chonTuyen - 1;

        while (true) {
            cout << CYAN << "\n   => CHON GIO XUAT PHAT CHO TUYEN NAY:\n" << RESET;
            cout << "      1. Chuyen 00:00 (Chay Cao toc CT01)\n";
            cout << "      2. Chuyen 06:00 (Chay Quoc lo QL20)\n";
            cout << "      3. Chuyen 12:00 (Chay Quoc lo QL20)\n";
            cout << "      4. Chuyen 18:00 (Chay Cao toc CT01)\n";
            cout << YELLOW << "   => Lua chon gio (1-4): " << RESET;
            if (cin >> chonGio && (chonGio >= 1 && chonGio <= 4)) break;
            cout << RED << "   [!] Gio khong hop le. Nhap lai!\n" << RESET;
            cin.clear(); cin.ignore(1000, '\n');
        }
        indexGio = chonGio - 1;

        int gioXuatPhat = 0;
        string duoiMaChuyen = "";
        if (indexGio == 0) { gioXuatPhat = 0;  loaiDuong = "CT01"; duoiMaChuyen = "00H"; }
        if (indexGio == 1) { gioXuatPhat = 6;  loaiDuong = "QL20"; duoiMaChuyen = "06H"; }
        if (indexGio == 2) { gioXuatPhat = 12; loaiDuong = "QL20"; duoiMaChuyen = "12H"; }
        if (indexGio == 3) { gioXuatPhat = 18; loaiDuong = "CT01"; duoiMaChuyen = "18H"; }

        if (indexTuyen == 0) {
            diemDi = "TP.HCM"; diemDen = "DA LAT"; maChuyen = "HCM" + duoiMaChuyen;
        } else {
            diemDi = "DA LAT"; diemDen = "TP.HCM"; maChuyen = "DL" + duoiMaChuyen;
        }

        int idxDon = 0, idxTra = 0;
        while (true) {
            if (loaiDuong == "CT01") {
                cout << GREEN << "\n   [*] Tuyen CT01 chay thang: Don/Tra mac dinh tai Nha xe N18 suot chang (300km).\n" << RESET;
                idxDon = (indexTuyen == 0) ? 0 : (int)danhSachTram.size() - 1;
                idxTra = (indexTuyen == 0) ? (int)danhSachTram.size() - 1 : 0;
                break;
            } else {
                cout << CYAN << "\n   --- DANH SACH CAC TRAM DUNG DOC DUONG QL20 ---\n" << RESET;
                for (size_t i = 0; i < danhSachTram.size(); i++) {
                    cout << "      " << right << setw(2) << i + 1 << ". " << left << setw(15) << danhSachTram[i].tenTram << " (" << danhSachTram[i].kmTuHCM << " km)\n";
                }
                cout << YELLOW << "   => Chon STT diem DON xe: " << RESET; cin >> idxDon; idxDon--;
                cout << YELLOW << "   => Chon STT diem TRA xe: " << RESET; cin >> idxTra; idxTra--;

                if (idxDon < 0 || idxDon >= (int)danhSachTram.size() || idxTra < 0 || idxTra >= (int)danhSachTram.size() || idxDon == idxTra) {
                    cout << RED << "   [!] Diem chon khong hop le hoac trung nhau. Chon lai!\n" << RESET; continue;
                }
                if (indexTuyen == 0 && idxDon > idxTra) {
                    cout << RED << "   [!] LOI: Ban chon nguoc duong hanh trinh di Da Lat. Chon lai!\n" << RESET; continue;
                }
                if (indexTuyen == 1 && idxDon < idxTra) {
                    cout << RED << "   [!] LOI: Ban chon nguoc duong hanh trinh ve TP.HCM. Chon lai!\n" << RESET; continue;
                }
                break;
            }
        }
        diemDon = danhSachTram[idxDon].tenTram;
        diemTra = danhSachTram[idxTra].tenTram;
        khoangCach = abs(danhSachTram[idxDon].kmTuHCM - danhSachTram[idxTra].kmTuHCM);

        int sTuBenGoc = (indexTuyen == 0) ? danhSachTram[idxDon].kmTuHCM : (300 - danhSachTram[idxDon].kmTuHCM);
        double tDiChuyen = (double)sTuBenGoc / 54.0;
        double tongGio = gioXuatPhat + tDiChuyen;
        int gioDon = (int)tongGio % 24;
        int phutDon = (int)((tongGio - (int)tongGio) * 60 + 0.5);
        stringstream ss; ss << setfill('0') << setw(2) << gioDon << ":" << setw(2) << phutDon;
        gioDonXe = ss.str();

        int loaiXeChon;
        while (true) {
            cout << CYAN << "\n   [ BUOC 3: CHON LOAI XE ]\n" << RESET;
            cout << "   1. Ghe ngoi    (1500d/km)\n";
            cout << "   2. Giuong nam (3000d/km)\n";
            cout << YELLOW << "   => Lua chon (1-2): " << RESET;
            if (cin >> loaiXeChon && (loaiXeChon == 1 || loaiXeChon == 2)) break;
            cin.clear(); cin.ignore(1000, '\n');
        }

        string loaiStr = (loaiXeChon == 1) ? "Ghe ngoi" : "Giuong nam";
        int xichYeuCau = (indexGio + indexTuyen) % 2; 
        vector<int> dsXePhuHop;

        cout << CYAN << "\n   --- DANH SACH XE DANG O DUNG DAU BEN & TRONG KHUNG GIO NAY ---\n" << RESET;
        for (int i = 0; i < (int)khoXe.size(); i++) {
            if (khoXe[i].loaiXe == loaiStr) {
                if (khoXe[i].maXichChay == -1 || khoXe[i].maXichChay == xichYeuCau) {
                    dsXePhuHop.push_back(i);
                    cout << "   " << dsXePhuHop.size() << ". Bien so: " << GREEN << khoXe[i].bienSo << RESET 
                         << " | So ghe con trong: " << YELLOW << khoXe[i].gheTrong[indexGio] << "/" << khoXe[i].tongGhe << RESET << "\n";
                }
            }
        }

        if(dsXePhuHop.empty()) {
            cout << RED << "   [!] Hien tai het xe phu hop tai dau ben nay de dieu dong!\n" << RESET;
            throw runtime_error("Het xe.");
        }

        int chonXe;
        while (true) {
            cout << YELLOW << "   => Chon STT xe muon dat: " << RESET;
            if (cin >> chonXe && chonXe >= 1 && chonXe <= (int)dsXePhuHop.size()) {
                indexXe = dsXePhuHop[chonXe - 1];
                break;
            }
            cout << RED << "   [!] Lua chon xe sai. Nhap lai!\n" << RESET;
        }

        while (true) {
            cout << YELLOW << "   => Nhap so luong ve can dat: " << RESET;
            if (cin >> soLuongVe && soLuongVe > 0 && soLuongVe <= khoXe[indexXe].gheTrong[indexGio]) {
                khoXe[indexXe].gheTrong[indexGio] -= soLuongVe; 
                khoXe[indexXe].maXichChay = xichYeuCau; 
                break;
            }
            cout << RED << "   [!] Ghe trong khong du hoac nhap sai! Thu lai.\n" << RESET;
            cin.clear(); cin.ignore(1000, '\n');
        }
    }

    int getIndexXe() { return indexXe; }
    int getSoLuongVe() { return soLuongVe; }
    int getGiaMotVe() { return khoangCach * khoXe[indexXe].donGia; }
    string getMaChuyen() { return maChuyen; }
    string getLoaiDuong() { return loaiDuong; }
    string getDiemDi() { return diemDi; }
    string getDiemDen() { return diemDen; }
    string getDiemDon() { return diemDon; }
    string getDiemTra() { return diemTra; }
    string getGioDonXe() { return gioDonXe; }
    int getKhoangCach() { return khoangCach; }
    int getIndexGio() { return indexGio; }

    void setChiTietFile(string mc, string dd, string dden, string ld, string ddon, string dtra, string gdon, int kc, int sl, int idxX, int idxG) {
        maChuyen = mc; diemDi = dd; diemDen = dden; loaiDuong = ld; diemDon = ddon;
        diemTra = dtra; gioDonXe = gdon; khoangCach = kc; soLuongVe = sl; indexXe = idxX;
        indexGio = idxG; indexTuyen = (mc.substr(0, 3) == "HCM") ? 0 : 1;
    }
};

// --- LOP VE XE ---
class VeXe {
private:
    string maVe;
    Nguoi* khachHang;
    ChuyenXe chuyenXe;
    bool coTrungChuyen;
public:
    VeXe() { khachHang = new KhachHang(); coTrungChuyen = false; }
    VeXe(const VeXe& o) {
        maVe = o.maVe; chuyenXe = o.chuyenXe; coTrungChuyen = o.coTrungChuyen;
        khachHang = new KhachHang(*static_cast<KhachHang*>(o.khachHang));
    }
    VeXe& operator=(const VeXe& o) {
        if (this != &o) {
            delete khachHang;
            maVe = o.maVe; chuyenXe = o.chuyenXe; coTrungChuyen = o.coTrungChuyen;
            khachHang = new KhachHang(*static_cast<KhachHang*>(o.khachHang));
        }
        return *this;
    }
    ~VeXe() { delete khachHang; }

    void nhap(int stt) {
        cout << BLUE << "\n================ DAT VE XE LUOT THU " << stt << " ================\n" << RESET;
        khachHang->nhap();
        try {
            chuyenXe.nhap();
        } catch (...) {
            throw; 
        }

        // --- TRICH XUAT 3 KY TU SO CUOI CUA BIEN SO ---
        string bs = khoXe[chuyenXe.getIndexXe()].bienSo;
        string chiSo = "";
        
        for (char c : bs) {
            if (isdigit(c)) {
                chiSo += c;
            }
        }
        
        string baSoCuoi = "000";
        if (chiSo.length() >= 3) {
            baSoCuoi = chiSo.substr(chiSo.length() - 3);
        } else if (!chiSo.empty()) {
            baSoCuoi = string(3 - chiSo.length(), '0') + chiSo;
        }

        stringstream ss; 
        ss << "N18-" << baSoCuoi << "-" << setfill('0') << setw(3) << stt;
        maVe = ss.str();

        string diemTra = chuyenXe.getDiemTra();
        
        if (diemTra == "TP.HCM" || diemTra == "DA LAT") {
            int tcOpt;
            while (true) {
                cout << YELLOW << "\n   => Ban xuong tai nha xe (" << diemTra << "). Co muon thue xe trung chuyen tan noi (30,000d/ve)? (1: Co | 0: Khong): " << RESET;
                if (cin >> tcOpt && (tcOpt == 0 || tcOpt == 1)) {
                    coTrungChuyen = (tcOpt == 1);
                    break;
                }
                cin.clear(); cin.ignore(1000, '\n');
            }
        } else {
            coTrungChuyen = false; 
            cout << GREEN << "\n   [*] LUU Y: Ban xuong xe doc duong tai tram [" << diemTra << "]. He thong KHONG ho tro dich vu xe trung chuyen!\n" << RESET;
        }
    }
    void xuat() {
        long long tongTien = (long long)chuyenXe.getGiaMotVe() * chuyenXe.getSoLuongVe();
        if (coTrungChuyen) tongTien += (30000LL * chuyenXe.getSoLuongVe());

        cout << GREEN << "\n\t--------------------------------------------------" << RESET;
        cout << BLUE  << "\n\t              BIEN LAI DAT VE XE KHACH            " << RESET;
        cout << GREEN << "\n\t--------------------------------------------------" << RESET;
        cout << "\n\t  * Ma ve: " << MAGENTA << maVe << RESET;
        cout << "\n\t  * Khach hang: " << khachHang->getHoTen();
        cout << "\n\t  * So dien thoai: " << khachHang->getSoDienThoai();
        cout << "\n\t  * Lo trinh: " << chuyenXe.getDiemDon() << " -> " << YELLOW << chuyenXe.getDiemTra() << RESET;
        cout << "\n\t  * Khoang cach: " << chuyenXe.getKhoangCach() << " km";
        cout << "\n\t  * Gio xe don du kien: " << YELLOW << chuyenXe.getGioDonXe() << RESET;
        cout << "\n\t  * So luong ve mua: " << chuyenXe.getSoLuongVe() << " ve";
        cout << "\n\t  * Dich vu trung chuyen: " << (coTrungChuyen ? "Co (30,000 d/ve)" : "Khong");
        cout << GREEN << "\n\t  ------------------------------------------------" << RESET;
        cout << "\n\t  => TONG TIEN THANH TOAN: " << GREEN << tongTien << " VND" << RESET;
        cout << GREEN << "\n\t--------------------------------------------------" << RESET;
        cout << GREEN << "\n   [OK] Dat ve thanh cong! Kho ghe he thong da cap nhat.\n" << RESET;
    }
    
    int getGia() { return chuyenXe.getGiaMotVe(); }
    string getMaVe() { return maVe; }
    string getTenKhach() { return khachHang->getHoTen(); }
    
    void hoanGhe() {
        int idxX = chuyenXe.getIndexXe();
        khoXe[idxX].gheTrong[chuyenXe.getIndexGio()] += chuyenXe.getSoLuongVe();
        
        bool trongToanBo = true;
        for (int i = 0; i < 4; i++) {
            if (khoXe[idxX].gheTrong[i] != khoXe[idxX].tongGhe) {
                trongToanBo = false; break;
            }
        }
        if (trongToanBo) khoXe[idxX].maXichChay = -1; 
    }

    void ghiFile(ofstream& f) {
        f << maVe << "," << khachHang->getHoTen() << "," << khachHang->getSoDienThoai() << ","
          << chuyenXe.getMaChuyen() << "," << chuyenXe.getLoaiDuong() << ","
          << chuyenXe.getDiemDi() << "," << chuyenXe.getDiemDen() << ","
          << chuyenXe.getDiemDon() << "," << chuyenXe.getDiemTra() << ","
          << chuyenXe.getGioDonXe() << "," << chuyenXe.getKhoangCach() << ","
          << chuyenXe.getGiaMotVe() << "," << chuyenXe.getSoLuongVe() << ","
          << (coTrungChuyen ? 1 : 0) << "," << chuyenXe.getIndexXe() << "," << chuyenXe.getIndexGio() << "\n";
    }

    void docTuFileToken(const vector<string>& tokens) {
        maVe = tokens[0]; khachHang->setHoTen(tokens[1]); static_cast<KhachHang*>(khachHang)->setSoDienThoai(tokens[2]);
        string mc = tokens[3], ld = tokens[4], dd = tokens[5], dden = tokens[6], ddon = tokens[7], dtra = tokens[8], gdon = tokens[9];
        int kc = stoi(tokens[10]), sl = stoi(tokens[12]);
        coTrungChuyen = (stoi(tokens[13]) == 1);
        int idxX = stoi(tokens[14]), idxG = stoi(tokens[15]);

        chuyenXe.setChiTietFile(mc, dd, dden, ld, ddon, dtra, gdon, kc, sl, idxX, idxG);
        khoXe[idxX].gheTrong[idxG] -= sl; 
        int tIdx = (mc.substr(0, 3) == "HCM") ? 0 : 1;
        khoXe[idxX].maXichChay = (idxG + tIdx) % 2; 
    }
};

// --- LOP QUAN LY TONG HOP ---
template <typename T>
class QuanLy {
private:
    vector<T> danhSach;
public:
    void them(T obj) { danhSach.push_back(obj); }
    int laySTT() { return danhSach.size() + 1; }

    void xuatTatCa() {
        if (danhSach.empty()) { cout << RED << "\n   [!] He thong hien tai chua co ve nao duoc dat.\n" << RESET; return; }
        string gachNgang = "===========================================================================================================================================================";
        cout << BLUE << "\n" << string(55, ' ') << "[ BANG HIEN THI DANH SACH VE VE XE DA DAT ]\n" << RESET;
        cout << CYAN << gachNgang << "\n";
        cout << "| " << left << setw(11) << "MA VE" << " | " << setw(16) << "HO TEN KHACH" << " | " << setw(11) << "SDT" << " | " << setw(7) << "MA CH." << " | " << setw(5) << "DUONG" << " | " << setw(13) << "DIEM BAT" << " | " << setw(13) << "DIEM XUONG" << " | " << setw(7) << "K_CACH" << " | " << setw(7) << "GIO DON" << " | " << right << setw(6) << "GIA/V" << " | " << setw(2) << "SL" << " | " << left << setw(5) << "TC" << " | " << right << setw(10) << "TONG TIEN" << " |\n";
        cout << gachNgang << "\n" << RESET;
        for (auto& item : danhSach) item.xuat();
        cout << CYAN << gachNgang << "\n" << RESET;
    }

    void sapXepGia() {
        sort(danhSach.begin(), danhSach.end(), [](T& a, T& b) { return a.getGia() < b.getGia(); });
    }

    void xoaVe(string ma) {
        for (auto it = danhSach.begin(); it != danhSach.end(); it++) {
            if (it->getMaVe() == ma) {
                it->hoanGhe(); 
                danhSach.erase(it);
                cout << GREEN << "\n   [OK] Da HUY thanh cong ma ve " << ma << " va hoan lai ghe vao kho xe!\n" << RESET;
                return;
            }
        }
        cout << RED << "\n   [!] Khong tim thay ma ve nay tren he thong!\n" << RESET;
    }

    void timKiemVe(string tuKhoa) {
        bool timThay = false;
        string gachNgang = "===========================================================================================================================================================";
        cout << BLUE << "\n" << string(55, ' ') << "[ KET QUA TIM KIEM VE XE ]\n" << RESET;
        cout << CYAN << gachNgang << "\n";
        cout << "| " << left << setw(11) << "MA VE" << " | " << setw(16) << "HO TEN KHACH" << " | " << setw(11) << "SDT" << " | " << setw(7) << "MA CH." << " | " << setw(5) << "DUONG" << " | " << setw(13) << "DIEM BAT" << " | " << setw(13) << "DIEM XUONG" << " | " << setw(7) << "K_CACH" << " | " << setw(7) << "GIO DON" << " | " << right << setw(6) << "GIA/V" << " | " << setw(2) << "SL" << " | " << left << setw(5) << "TC" << " | " << right << setw(10) << "TONG TIEN" << " |\n";
        cout << gachNgang << "\n" << RESET;
        
        for (auto& item : danhSach) {
            if (item.getMaVe().find(tuKhoa) != string::npos || item.getTenKhach().find(tuKhoa) != string::npos) {
                item.xuat(); timThay = true;
            }
        }
        cout << CYAN << gachNgang << "\n" << RESET;
        if (!timThay) cout << RED << "   [!] Khong tim thay bat ky ket qua nao cho tu khoa '" << tuKhoa << "'.\n" << RESET;
    }

    void luuFile(string tenFile) {
        ofstream f(tenFile); if (!f) return;
        f << danhSach.size() << "\n";
        for (auto& item : danhSach) item.ghiFile(f);
        f.close();
    }

    void docFile(string tenFile) {
        ifstream f(tenFile); if (!f) return;
        string line; if (!getline(f, line)) return;
        danhSach.clear();
        while (getline(f, line)) {
            if (line.empty()) continue;
            stringstream ss(line); string token; vector<string> tokens;
            while (getline(ss, token, ',')) tokens.push_back(token);
            if (tokens.size() >= 16) {
                T item; item.docTuFileToken(tokens); danhSach.push_back(item);
            }
        }
        f.close();
    }
};

int main() {
    QuanLy<VeXe> ql;
    ql.docFile("data.csv"); 

    int chonLua;
    while (true) {
        cout << BLUE << "\n      ========== HE THONG DIEU HANH QUAN LY VE XE N18 =========\n" << RESET;
        cout << CYAN << "      | " << RESET << "  1. Dat ve xe moi                                    " << CYAN << "|\n" << RESET;
        cout << CYAN << "      | " << RESET << "  2. In toan bo danh sach ve da dat                   " << CYAN << "|\n" << RESET;
        cout << CYAN << "      | " << RESET << "  3. Sap xep danh sach ve theo gia tang dan           " << CYAN << "|\n" << RESET;
        cout << CYAN << "      | " << RESET << "  4. Tim kiem ve theo Ma ve / Ten khach hang          " << CYAN << "|\n" << RESET;
        cout << CYAN << "      | " << RESET << "  5. Huy / Xoa ve xe khoi he thong                    " << CYAN << "|\n" << RESET;
        cout << CYAN << "      | " << RESET << "  6. Luu thu cong du lieu hien tai ra file 'data.csv' " << CYAN << "|\n" << RESET;
        cout << CYAN << "      | " << RESET << "  0. Thoat chuong trinh                               " << CYAN << "|\n" << RESET;
        cout << CYAN << "      =========================================================\n" << RESET;
        cout << YELLOW << "      => Nhap lua chon: " << RESET;
        if (!(cin >> chonLua)) { cin.clear(); cin.ignore(1000, '\n'); continue; }
        cin.ignore(1000, '\n');
        if (chonLua == 0) break;

        switch (chonLua) {
            case 1: {
                try {
                    VeXe v; 
                    v.nhap(ql.laySTT()); 
                    v.xuat();       
                    ql.them(v);          
                } catch(...) {}
                break;
            }
            case 2: ql.xuatTatCa(); break;
            case 3: ql.sapXepGia(); cout << GREEN << "\n   [OK] Da sap xep xong toan bo ve theo gia ve.\n" << RESET; break;
            case 4: {
                string tuKhoa; cout << YELLOW << "   => Nhap Ma ve hoac Ten khach hang can tim: " << RESET;
                getline(cin, tuKhoa); ql.timKiemVe(tuKhoa); break;
            }
            case 5: {
                string maVeXoa; cout << YELLOW << "   => Nhap Ma ve can xoa khoi he thong: " << RESET;
                getline(cin, maVeXoa); ql.xoaVe(maVeXoa); break;
            }
            case 6: ql.luuFile("data.csv"); cout << GREEN << "\n   [OK] Du lieu va kho ghe da duoc dong bo chat che.\n" << RESET; break;
            default: cout << RED << "   [!] Tinh nang khong hop le. Chon tu 0 -> 6.\n" << RESET;
        }
    }
    return 0;
}
