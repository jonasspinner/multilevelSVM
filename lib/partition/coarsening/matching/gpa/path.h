/******************************************************************************
 * path.h
 *
 * Source of KaHIP -- Karlsruhe High Quality Partitioning.
 *
 ******************************************************************************
 * Copyright (C) 2013-2015 Christian Schulz <christian.schulz@kit.edu>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#ifndef PATH_X5LQS3DT
#define PATH_X5LQS3DT

#include "definitions.h"

class path {
  public:
    path() = default;

    constexpr explicit path(const NodeID &v) : head(v), tail(v), length(0), active(true) {}

    void init(const NodeID &v) { *this = path(v); }

    [[nodiscard]] NodeID get_tail() const;

    void set_tail(const NodeID &v);

    [[nodiscard]] NodeID get_head() const;

    void set_head(const NodeID &v);

    void set_length(const EdgeID &length);

    [[nodiscard]] EdgeID get_length() const;

    // returns weather the path is a cycle or not.
    [[nodiscard]] bool is_cycle() const;

    [[nodiscard]] bool is_active() const;

    void set_active(bool active);

  private:
    // Last vertex of the path. Cycles have head == tail
    NodeID head{UNDEFINED_NODE};

    // First vertex of the path. Cycles have head == tail
    NodeID tail{UNDEFINED_NODE};

    // Number of edges in the graph
    EdgeID length{0};

    // True iff the parth is still in use. False iff it has been removed.
    bool active{false};
};

inline NodeID path::get_tail() const { return tail; }

inline void path::set_tail(const NodeID &v) { tail = v; }

inline NodeID path::get_head() const { return head; }

inline void path::set_head(const NodeID &v) { head = v; }

inline EdgeID path::get_length() const { return length; }

inline void path::set_length(const EdgeID &len) { length = len; }

inline bool path::is_cycle() const { return (head == tail) and (length > 0); }

inline bool path::is_active() const { return active; }

inline void path::set_active(const bool act) { active = act; }

#endif /* end of include guard: PATH_X5LQS3DT */
