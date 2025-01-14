------------------------------------------------------------------------------
--                                                                          --
--                         GNAT LIBRARY COMPONENTS                          --
--                                                                          --
--                 ADA.CONTAINERS.FORMAL_DOUBLY_LINKED_LISTS                --
--                                                                          --
--                                 S p e c                                  --
--                                                                          --
--          Copyright (C) 2004-2013, Free Software Foundation, Inc.         --
--                                                                          --
-- This specification is derived from the Ada Reference Manual for use with --
-- GNAT. The copyright notice above, and the license provisions that follow --
-- apply solely to the  contents of the part following the private keyword. --
--                                                                          --
-- GNAT is free software;  you can  redistribute it  and/or modify it under --
-- terms of the  GNU General Public License as published  by the Free Soft- --
-- ware  Foundation;  either version 3,  or (at your option) any later ver- --
-- sion.  GNAT is distributed in the hope that it will be useful, but WITH- --
-- OUT ANY WARRANTY;  without even the  implied warranty of MERCHANTABILITY --
-- or FITNESS FOR A PARTICULAR PURPOSE.                                     --
--                                                                          --
-- As a special exception under Section 7 of GPL version 3, you are granted --
-- additional permissions described in the GCC Runtime Library Exception,   --
-- version 3.1, as published by the Free Software Foundation.               --
--                                                                          --
-- You should have received a copy of the GNU General Public License and    --
-- a copy of the GCC Runtime Library Exception along with this program;     --
-- see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see    --
-- <http://www.gnu.org/licenses/>.                                          --
------------------------------------------------------------------------------

--  This spec is derived from Ada.Containers.Bounded_Doubly_Linked_Lists in the
--  Ada 2012 RM. The modifications are meant to facilitate formal proofs by
--  making it easier to express properties, and by making the specification of
--  this unit compatible with SPARK 2014. Note that the API of this unit may be
--  subject to incompatible changes as SPARK 2014 evolves.

--  The modifications are:

--    A parameter for the container is added to every function reading the
--    contents of a container: Next, Previous, Query_Element, Has_Element,
--    Iterate, Reverse_Iterate, Element. This change is motivated by the need
--    to have cursors which are valid on different containers (typically a
--    container C and its previous version C'Old) for expressing properties,
--    which is not possible if cursors encapsulate an access to the underlying
--    container.

--    There are three new functions:

--      function Strict_Equal (Left, Right : List) return Boolean;
--      function Left  (Container : List; Position : Cursor) return List;
--      function Right (Container : List; Position : Cursor) return List;

--    See subprogram specifications that follow for details

generic
   type Element_Type is private;

   with function "=" (Left, Right : Element_Type)
                      return Boolean is <>;

package Ada.Containers.Formal_Doubly_Linked_Lists is
   pragma Pure;

   type List (Capacity : Count_Type) is private;
   pragma Preelaborable_Initialization (List);

   type Cursor is private;
   pragma Preelaborable_Initialization (Cursor);

   Empty_List : constant List;

   No_Element : constant Cursor;

   function "=" (Left, Right : List) return Boolean;

   function Length (Container : List) return Count_Type;

   function Is_Empty (Container : List) return Boolean;

   procedure Clear (Container : in out List);

   procedure Assign (Target : in out List; Source : List) with
     Pre => Target.Capacity >= Length (Source);

   function Copy (Source : List; Capacity : Count_Type := 0) return List;

   function Element
     (Container : List;
      Position : Cursor) return Element_Type
   with
     Pre => Has_Element (Container, Position);

   procedure Replace_Element
     (Container : in out List;
      Position  : Cursor;
      New_Item  : Element_Type)
   with
     Pre => Has_Element (Container, Position);

   procedure Move (Target : in out List; Source : in out List) with
     Pre => Target.Capacity >= Length (Source);

   procedure Insert
     (Container : in out List;
      Before    : Cursor;
      New_Item  : Element_Type;
      Count     : Count_Type := 1)
   with
     Pre => Length (Container) + Count <= Container.Capacity
              and then (Has_Element (Container, Before)
                         or else Before = No_Element);

   procedure Insert
     (Container : in out List;
      Before    : Cursor;
      New_Item  : Element_Type;
      Position  : out Cursor;
      Count     : Count_Type := 1)
   with
     Pre => Length (Container) + Count <= Container.Capacity
              and then (Has_Element (Container, Before)
                         or else Before = No_Element);

   procedure Insert
     (Container : in out List;
      Before    : Cursor;
      Position  : out Cursor;
      Count     : Count_Type := 1)
   with
     Pre => Length (Container) + Count <= Container.Capacity
              and then (Has_Element (Container, Before)
                         or else Before = No_Element);

   procedure Prepend
     (Container : in out List;
      New_Item  : Element_Type;
      Count     : Count_Type := 1)
   with
     Pre => Length (Container) + Count <= Container.Capacity;

   procedure Append
     (Container : in out List;
      New_Item  : Element_Type;
      Count     : Count_Type := 1)
   with
     Pre => Length (Container) + Count <= Container.Capacity;

   procedure Delete
     (Container : in out List;
      Position  : in out Cursor;
      Count     : Count_Type := 1)
   with
     Pre => Has_Element (Container, Position);

   procedure Delete_First
     (Container : in out List;
      Count     : Count_Type := 1);

   procedure Delete_Last
     (Container : in out List;
      Count     : Count_Type := 1);

   procedure Reverse_Elements (Container : in out List);

   procedure Swap
     (Container : in out List;
      I, J      : Cursor)
   with
     Pre => Has_Element (Container, I) and then Has_Element (Container, J);

   procedure Swap_Links
     (Container : in out List;
      I, J      : Cursor)
   with
     Pre => Has_Element (Container, I) and then Has_Element (Container, J);

   procedure Splice
     (Target : in out List;
      Before : Cursor;
      Source : in out List)
   with
     Pre => Length (Source) + Length (Target) <= Target.Capacity
              and then (Has_Element (Target, Before)
                         or else Before = No_Element);

   procedure Splice
     (Target   : in out List;
      Before   : Cursor;
      Source   : in out List;
      Position : in out Cursor)
   with
     Pre => Length (Source) + Length (Target) <= Target.Capacity
              and then (Has_Element (Target, Before)
                         or else Before = No_Element)
              and then Has_Element (Source, Position);

   procedure Splice
     (Container : in out List;
      Before    : Cursor;
      Position  : Cursor)
   with
     Pre => 2 * Length (Container) <= Container.Capacity
              and then (Has_Element (Container, Before)
                         or else Before = No_Element)
              and then Has_Element (Container, Position);

   function First (Container : List) return Cursor;

   function First_Element (Container : List) return Element_Type with
     Pre => not Is_Empty (Container);

   function Last (Container : List) return Cursor;

   function Last_Element (Container : List) return Element_Type with
     Pre => not Is_Empty (Container);

   function Next (Container : List; Position : Cursor) return Cursor with
     Pre => Has_Element (Container, Position) or else Position = No_Element;

   procedure Next (Container : List; Position : in out Cursor) with
     Pre => Has_Element (Container, Position) or else Position = No_Element;

   function Previous (Container : List; Position : Cursor) return Cursor with
     Pre => Has_Element (Container, Position) or else Position = No_Element;

   procedure Previous (Container : List; Position : in out Cursor) with
     Pre => Has_Element (Container, Position) or else Position = No_Element;

   function Find
     (Container : List;
      Item      : Element_Type;
      Position  : Cursor := No_Element) return Cursor
   with
     Pre => Has_Element (Container, Position) or else Position = No_Element;

   function Reverse_Find
     (Container : List;
      Item      : Element_Type;
      Position  : Cursor := No_Element) return Cursor
   with
     Pre => Has_Element (Container, Position) or else Position = No_Element;

   function Contains
     (Container : List;
      Item      : Element_Type) return Boolean;

   function Has_Element (Container : List; Position : Cursor) return Boolean;

   generic
      with function "<" (Left, Right : Element_Type) return Boolean is <>;
   package Generic_Sorting is

      function Is_Sorted (Container : List) return Boolean;

      procedure Sort (Container : in out List);

      procedure Merge (Target, Source : in out List);

   end Generic_Sorting;

   function Strict_Equal (Left, Right : List) return Boolean;
   --  Strict_Equal returns True if the containers are physically equal, i.e.
   --  they are structurally equal (function "=" returns True) and that they
   --  have the same set of cursors.

   function Left  (Container : List; Position : Cursor) return List with
     Pre => Has_Element (Container, Position) or else Position = No_Element;
   function Right (Container : List; Position : Cursor) return List with
     Pre => Has_Element (Container, Position) or else Position = No_Element;
   --  Left returns a container containing all elements preceding Position
   --  (excluded) in Container. Right returns a container containing all
   --  elements following Position (included) in Container. These two new
   --  functions can be used to express invariant properties in loops which
   --  iterate over containers. Left returns the part of the container already
   --  scanned and Right the part not scanned yet.

private

   type Node_Type is record
      Prev    : Count_Type'Base := -1;
      Next    : Count_Type;
      Element : Element_Type;
   end record;

   function "=" (L, R : Node_Type) return Boolean is abstract;

   type Node_Array is array (Count_Type range <>) of Node_Type;
   function "=" (L, R : Node_Array) return Boolean is abstract;

   type List (Capacity : Count_Type) is tagged record
      Nodes  : Node_Array (1 .. Capacity) := (others => <>);
      Free   : Count_Type'Base := -1;
      Length : Count_Type := 0;
      First  : Count_Type := 0;
      Last   : Count_Type := 0;
   end record;

   type Cursor is record
      Node : Count_Type := 0;
   end record;

   Empty_List : constant List := (0, others => <>);

   No_Element : constant Cursor := (Node => 0);

end Ada.Containers.Formal_Doubly_Linked_Lists;
