#pragma once
using namespace std;
class Container {
protected:
    string name = "container";
public:
    string getName() const { return name; }
    string virtual virtualToString() const { return name; }
    string toString() const { return name; }
};

template<typename type>
class Array : public Container {
private:
    type* arr;
    int n;
    int id;
    static int ñountObjects;
    static int TotalCreatedObjects;
public:
    // êîíñòğóêòîğ ïî óìîë÷àíèş
    Array() {
        name = "array<" + string(typeid(type).name()) + ">";
        arr = 0;
        n = 0;
        id = ñountObjects;
        ñountObjects++;
        TotalCreatedObjects++;
    }

    // êîíñòğóêòîğ èíèöèàëèçàöèè
    Array(int n) {
        id = ñountObjects;
        ñountObjects++;
        this->n = n;
        arr = new type[n];
        for (int i = 0; i < n; i++)
            arr[i] = type();
    }

    // êîíñòğóêòîğ êîïèğîâàíèÿ
    Array(const Array& other) {
        n = other.n;
        id = ñountObjects;
        ñountObjects++;
        arr = new type[n];
        for (int i = 0; i < n; i++) {
            arr[i] = other.arr[i];
        }
    }

    // äåñòğóêòîğ
    ~Array() {
        delete[] arr;
        ñountObjects--;
    }

    // îïåğàòîğ ïğèñâàèâàíèÿ
    Array& operator=(const Array& get_specifiers_from)
    {
        if (this != &get_specifiers_from)
        {
            n = get_specifiers_from.n;
            delete[]arr;
            arr = new type[n];
            for (int i = 0; i < n; i++)
            {
                arr[i] = get_specifiers_from.arr[i];
            }
        }
        return *this;
    }


    type& operator[](int index) const
    {
        return arr[index];
    }

    /*type& operator()(int index)
    {
        return arr[index];
    }*/




    bool equal(Array tnew)
    {
        for (int i = 0; i < n; i++)
        {
            if (arr[i] != tnew.arr[i])
                return false;
        }
        return true;
    }

    bool operator==(const Array& get_specifiers_from)
    {
        if (this != &get_specifiers_from)
        {
            if (n == get_specifiers_from.n)
            {
                if (equal(get_specifiers_from))
                    return true;
            }
            return false;

        }
        return true;
    }

    bool operator !=(const Array& get_specifiers_from)
    {
        if (*this == get_specifiers_from)
            return false;
        return true;
    }

    void push_back(type h)
    {
        type* arr1 = new type[n + 1];
        for (int i = 0; i < n; i++)
        {
            arr1[i] = arr[i];
        }

        arr1[n] = h;
        delete[]arr;
        arr = arr1;
        n++;
    }

    bool pop_back()
    {
        if (n > 0)
        {
            type* arr1 = new type[n - 1];
            for (int i = 0; i < n - 1; i++)
                arr1[i] = arr[i];;
            delete[]arr;
            arr = arr1;
            n--;
            return true;
        }
        return false;
    }


    void insert(const type& elem, int pos)
    {
        if (pos > 0 && pos <= n + 1)
        {
            if (n != 0)
            {
                type* arr1 = new type[n + 1];
                for (int i = 0; i < pos - 1; i++)
                {
                    arr1[i] = arr[i];
                }
                arr1[pos - 1] = elem;
                arr1[pos] = arr[pos - 1];

                for (int i = pos - 1; i < n; i++)
                {
                    arr1[i + 1] = arr[i];
                }
                delete[]arr;
                arr = arr1;
                n++;
            }
            else
            {
                type* arr1 = new type[n + 1];
                for (int i = 0; i < pos - 1; i++)
                {
                    arr1[i] = arr[i];
                }
                arr1[pos - 1] = elem;
                delete[]arr;
                arr = arr1;
                n++;
            }
        }

    }

    bool remove(int pos)
    {
        if (n > 0 && pos < n + 1 && pos>0)
        {
            type* arr1 = new type[n - 1];
            for (int i = 0; i < pos - 1; i++)
            {
                arr1[i] = arr[i];
            }
            for (int i = pos - 1; i < n - 1; i++)
            {
                arr1[i] = arr[i + 1];
            }

            delete[]arr;
            n--;
            arr = arr1;

            return true;

        }
        return false;
    }

    void show() const {
        cout << "id: " << id << endl;
        cout << "size: " << n << endl;
        for (int i = 0; i < n; i++) {
            cout << arr[i] << " ";
        }
        cout << endl;
    }

    friend ostream& operator<<(ostream& out, const Array& b)
    {
        out << "size of arr " << b.n << ' ' << "elems: ";
        for (int i = 0; i < b.n; i++)
        {
            out << b.arr[i] << ' ';
        }
        return out;
    }

    int size() const
    {
        return n;
    }

    static int getCountObjects() {
        return Array<type>::ñountObjects;
    }
    static int getTotalCreatedObjects()
    {
        return Array<type>::TotalCreatedObjects;
    }
};

template <typename T>
int Array<T>::TotalCreatedObjects = 0;

template <typename T>
int Array<T>::ñountObjects = 0;