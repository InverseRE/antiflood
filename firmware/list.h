/* -*-*- mode: c++ -*-*- */

/*
   Antiflood Copyright (C) 2018 Alexey <Inverse> Shumeiko

   This file is part of Antiflood project.

   This firmware is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This firmware is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with the firmware. If not, see <http://www.gnu.org/licenses/>.

   For more details see LICENSE file.
*/

#ifndef __LIST_H__
#define __LIST_H__

template <typename T>
class List {
    struct Node {
        T data;
        Node* next;

        Node(const T& d) : data(d), next(nullptr) {}
        Node(const T& d, Node* n) : data(d), next(n) {}
    };

private:
    Node* head;
    Node* curr;
    bool (*cmp)(const T&, const T&);

    static bool defcmp(const T& a, const T& b) { return a == b; }

public:
    List() : head(nullptr), curr(nullptr), cmp(defcmp) {}
    List(const T& d) : curr(nullptr), cmp(defcmp) { head = new Node(d); }
    List(const T& d, bool (*eq)(const T&, const T&)) : curr(nullptr), cmp(eq) { head = new Node(d); }

    ~List() {
        Node* c = head;
        while (c) {
            Node* d = c;
            c = c->next;
            delete d;
        }
    }

    bool add(const T& data) {
        curr = nullptr;

        Node* c = new Node(data, head);
        head = c ? c : head;
        return c;
    }

    bool drop(const T& sample) {
        curr = nullptr;

        Node* c = head;
        Node* p = nullptr;

        while (c) {
            if (cmp(sample, c->data)) {
                if (p) {
                    p->next = c->next;
                }
                if (head == c) {
                    head = c->next;
                }
                delete c;
                return true;
            }
            p = c;
            c = c->next;
        }

        return false;
    }

    T* get(const T& sample) const {
        Node* c = head;

        while (c) {
            if (cmp(sample, c->data)) {
                return &c->data;
            }
            c = c->next;
        }

        return nullptr;
    }

    T* start() {
        curr = head;
        return curr ? &curr->data : nullptr;
    }

    T* next() {
        if (curr) {
            curr = curr->next;
        }
        return curr ? &curr->data : nullptr;
    }
};

#endif  /* __LIST_H__ */
